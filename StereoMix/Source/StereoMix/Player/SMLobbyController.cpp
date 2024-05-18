// Copyright Surround, Inc. All Rights Reserved.


#include "SMLobbyController.h"

#include "TurboLinkGrpcManager.h"
#include "API/SMGrpcSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "SAuth/AuthService.h"

void ASMLobbyController::InitAuthService()
{
	AuthService = Cast<UAuthService>(GrpcSubsystem->GetManager()->MakeService(TEXT("AuthService")));
	AuthService->OnServiceStateChanged.AddUniqueDynamic(this, &ASMLobbyController::OnAuthServiceStateChanged);
}

void ASMLobbyController::InitLobbyService()
{
	LobbyService = Cast<ULobbyService>(GrpcSubsystem->GetManager()->MakeService(TEXT("LobbyService")));
	LobbyService->OnServiceStateChanged.AddUniqueDynamic(this, &ASMLobbyController::OnLobbyServiceStateChanged);
}

void ASMLobbyController::OnAuthServiceStateChanged(EGrpcServiceState State)
{
	UE_LOG(LogSMLobbyController, Log, TEXT("AuthServiceStateChanged: %s"), *UEnum::GetValueAsString(State));

	if (State == EGrpcServiceState::Ready && UserToken.IsEmpty())
	{
		UE_LOG(LogSMLobbyController, Log, TEXT("AuthService is ready. Trying to login as guest."));
		GuestLogin();
	}
}

void ASMLobbyController::OnLobbyServiceStateChanged(EGrpcServiceState State)
{
	UE_LOG(LogSMLobbyController, Log, TEXT("LobbyServiceStateChanged: %s"), *UEnum::GetValueAsString(State));
}

void ASMLobbyController::HandleGuestLoginResponse(FGrpcContextHandle ContextHandle, const FGrpcResult& Result, const FGrpcAuthLoginResponse& Response)
{
	if (Result.Code != EGrpcResultCode::Ok)
	{
		// Grpc Error
		UE_LOG(LogSMLobbyController, Error, TEXT("Failed to login as guest. (%s)"), *UEnum::GetValueAsString(Result.Code));
		OnReceiveLoginResponse(false);
		OnLoginResponse.Broadcast(false);
		return;
	}

	// Login Success
	UserToken = Response.Token;
	UE_LOG(LogSMLobbyController, Log, TEXT("Received JWT Token: %s"), *UserToken);
	UserMetaData.MetaData.Add("authorization", TEXT("Bearer ") + UserToken);
	bUserTokenReceived = true;

	OnReceiveLoginResponse(true);
	OnLoginResponse.Broadcast(true);
}

void ASMLobbyController::HandleCreateRoomResponse(FGrpcContextHandle ContextHandle, const FGrpcResult& Result, const FGrpcLobbyCreateRoomResponse& Response)
{
	FGrpcLobbyRoomConnectionInfo Connection;

	if (Result.Code != EGrpcResultCode::Ok)
	{
		UE_LOG(LogSMLobbyController, Error, TEXT("Failed to create room. (%s)"), *UEnum::GetValueAsString(Result.Code));
		OnReceiveCreateRoomResponse(false, Connection);
		OnCreateRoomResponse.Broadcast(false, Connection);
		return;
	}

	Connection.Host = Response.Connection.Host;
	Connection.Port = Response.Connection.Port;
	OnReceiveCreateRoomResponse(true, Connection);
	OnCreateRoomResponse.Broadcast(true, Connection);
}

void ASMLobbyController::HandleJoinRoomResponse(FGrpcContextHandle ContextHandle, const FGrpcResult& Result, const FGrpcLobbyJoinRoomResponse& Response)
{
	FGrpcLobbyRoomConnectionInfo Connection;

	if (Result.Code != EGrpcResultCode::Ok)
	{
		UE_LOG(LogSMLobbyController, Error, TEXT("Failed to join room. (%s)"), *UEnum::GetValueAsString(Result.Code));
		OnReceiveJoinRoomResponse(false, Connection);
		OnJoinRoomResponse.Broadcast(false, Connection);
		return;
	}

	Connection.Host = Response.Connection.Host;
	Connection.Port = Response.Connection.Port;
	OnReceiveJoinRoomResponse(true, Connection);
	OnJoinRoomResponse.Broadcast(true, Connection);
}

void ASMLobbyController::HandleGetRoomListResponse(FGrpcContextHandle ContextHandle, const FGrpcResult& Result, const FGrpcLobbyGetRoomListResponse& Response)
{
	if (Result.Code != EGrpcResultCode::Ok)
	{
		UE_LOG(LogSMLobbyController, Error, TEXT("Failed to get room list. (%s)"), *UEnum::GetValueAsString(Result.Code));
		const TArray<FGrpcLobbyRoom> EmptyRoomList;
		OnReceiveRoomListUpdateResponse(false, EmptyRoomList);
		OnRoomListUpdateResponse.Broadcast(false, EmptyRoomList);
		return;
	}

	OnReceiveRoomListUpdateResponse(true, Response.Rooms);
	OnRoomListUpdateResponse.Broadcast(true, Response.Rooms);
}

ASMLobbyController::ASMLobbyController()
{
}

void ASMLobbyController::BeginPlay()
{
	Super::BeginPlay();

	ShowLoadingScreen();
	CurrentWidget = CreateWidget<ULobbyUserWidget>(this, LobbyUI);
	CurrentWidget->AddToViewport();
	CurrentWidget->UpdateEnabled(false);

	GrpcSubsystem = GetGameInstance()->GetSubsystem<USMGrpcSubsystem>();
	InitAuthService();
	InitLobbyService();

	AuthServiceClient = AuthService->MakeClient();
	AuthServiceClient->OnGuestLoginResponse.AddUniqueDynamic(this, &ASMLobbyController::HandleGuestLoginResponse);

	LobbyServiceClient = LobbyService->MakeClient();
	LobbyServiceClient->OnCreateRoomResponse.AddUniqueDynamic(this, &ASMLobbyController::HandleCreateRoomResponse);
	LobbyServiceClient->OnJoinRoomResponse.AddUniqueDynamic(this, &ASMLobbyController::HandleJoinRoomResponse);
	LobbyServiceClient->OnGetRoomListResponse.AddUniqueDynamic(this, &ASMLobbyController::HandleGetRoomListResponse);

	AuthService->Connect();
	LobbyService->Connect();

	SetInputMode(FInputModeUIOnly());
}

void ASMLobbyController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (AuthServiceClient)
	{
		AuthServiceClient->Shutdown();
	}
	if (LobbyServiceClient)
	{
		LobbyServiceClient->Shutdown();
	}
}

void ASMLobbyController::ShowLoadingScreen()
{
	UE_LOG(LogSMLobbyController, Log, TEXT("ShowLoadingScreen"))
	LoadingScreenInstance = CreateWidget<UUserWidget>(this, LoadingScreenUI);
	if (!LoadingScreenInstance)
	{
		UE_LOG(LogSMLobbyController, Error, TEXT("Failed to create LoadingScreen widget"))
		return;
	}

	LoadingScreenInstance->AddToViewport(1000);
}

void ASMLobbyController::CloseLoadingScreen()
{
	UE_LOG(LogSMLobbyController, Log, TEXT("CloseLoadingScreen"))
	if (LoadingScreenInstance)
	{
		LoadingScreenInstance->RemoveFromParent();
		LoadingScreenInstance = nullptr;
	}
}

void ASMLobbyController::GuestLogin()
{
	if (AuthService->GetServiceState() != EGrpcServiceState::Ready && AuthService->GetServiceState() != EGrpcServiceState::Idle)
	{
		UE_LOG(LogSMLobbyController, Warning, TEXT("AuthService is not ready"))
		return;
	}

	if (AuthServiceClient->GetContextState(GuestLoginContext) == EGrpcContextState::Busy)
	{
		UE_LOG(LogSMLobbyController, Warning, TEXT("GuestLogin is already in progress"))
		return;
	}

	FGrpcAuthGuestLoginRequest Request;
	Request.UserName = "TestUser";
	GuestLoginContext = AuthServiceClient->InitGuestLogin();
	AuthServiceClient->GuestLogin(GuestLoginContext, Request);
}

void ASMLobbyController::CreateRoom(const FString& RoomName, EGrpcLobbyRoomVisibility Visibility, EGrpcLobbyGameMode Mode, EGrpcLobbyGameMap Map, const FString& Password)
{
	if (!bUserTokenReceived)
	{
		UE_LOG(LogSMLobbyController, Warning, TEXT("UserToken is not received yet"))
		return;
	}

	if (LobbyService->GetServiceState() != EGrpcServiceState::Ready && LobbyService->GetServiceState() != EGrpcServiceState::Idle)
	{
		UE_LOG(LogSMLobbyController, Warning, TEXT("LobbyService is not ready"))
		return;
	}

	if (LobbyServiceClient->GetContextState(CreateRoomContext) == EGrpcContextState::Busy)
	{
		UE_LOG(LogSMLobbyController, Warning, TEXT("CreateRoom is already in progress"))
		return;
	}

	CurrentWidget->SetIsEnabled(false);
	FGrpcLobbyCreateRoomRequest Request;

	Request.Config.RoomName = RoomName;
	Request.Password = Password;
	Request.Config.Visibility = Visibility;
	Request.Config.Mode = Mode;
	Request.Config.Map = Map;
	CreateRoomContext = LobbyServiceClient->InitCreateRoom();
	LobbyServiceClient->CreateRoom(CreateRoomContext, Request, UserMetaData);
}

void ASMLobbyController::JoinRoom(const FString& RoomId, const FString& Password)
{
	if (!bUserTokenReceived)
	{
		UE_LOG(LogSMLobbyController, Warning, TEXT("UserToken is not received yet"))
		return;
	}

	if (LobbyService->GetServiceState() != EGrpcServiceState::Ready && LobbyService->GetServiceState() != EGrpcServiceState::Idle)
	{
		UE_LOG(LogSMLobbyController, Warning, TEXT("LobbyService is not ready"))
		return;
	}

	if (LobbyServiceClient->GetContextState(JoinRoomContext) == EGrpcContextState::Busy)
	{
		UE_LOG(LogSMLobbyController, Warning, TEXT("CreateRoom is already in progress"))
		return;
	}

	CurrentWidget->SetIsEnabled(false);
	FGrpcLobbyJoinRoomRequest Request;
	Request.Id.IdCase = EGrpcLobbyJoinRoomRequestId::ShortRoomId;
	Request.Id.ShortRoomId = RoomId;
	Request.Password = Password;
	JoinRoomContext = LobbyServiceClient->InitJoinRoom();
	LobbyServiceClient->JoinRoom(JoinRoomContext, Request, UserMetaData);
}

void ASMLobbyController::GetRoomList(EGrpcLobbyRoomVisibility Visibility, EGrpcLobbyGameMode Mode, EGrpcLobbyGameMap Map)
{
	if (!bUserTokenReceived)
	{
		UE_LOG(LogSMLobbyController, Warning, TEXT("UserToken is not received yet"))
		return;
	}

	if (LobbyService->GetServiceState() != EGrpcServiceState::Ready && LobbyService->GetServiceState() != EGrpcServiceState::Idle)
	{
		UE_LOG(LogSMLobbyController, Warning, TEXT("LobbyService is not ready"))
		return;
	}

	if (LobbyServiceClient->GetContextState(GetRoomListContext) == EGrpcContextState::Busy)
	{
		UE_LOG(LogSMLobbyController, Warning, TEXT("CreateRoom is already in progress"))
		return;
	}

	FGrpcLobbyGetRoomListRequest Request;
	Request.Visibility = Visibility;
	Request.Mode = Mode;
	Request.Map = Map;
	GetRoomListContext = LobbyServiceClient->InitGetRoomList();
	LobbyServiceClient->GetRoomList(GetRoomListContext, Request, UserMetaData);
}

void ASMLobbyController::OnReceiveLoginResponse(bool Success)
{
	CurrentWidget->UpdateEnabled(Success);

	if (Success)
	{
		CloseLoadingScreen();
	}
	else
	{
		SwitchLevel("/Game/StereoMix/Levels/Lobby/L_Title_2");
	}
}

void ASMLobbyController::OnReceiveCreateRoomResponse(bool Success, const FGrpcLobbyRoomConnectionInfo& ConnectionInfo)
{
	if (!Success)
	{
		CurrentWidget->SetIsEnabled(true);
		return;
	}

	ShowLoadingScreen();
	ClientTravel(FString::Printf(TEXT("%s:%d"), *ConnectionInfo.Host, ConnectionInfo.Port), TRAVEL_Absolute);
}

void ASMLobbyController::OnReceiveJoinRoomResponse(bool Success, const FGrpcLobbyRoomConnectionInfo& ConnectionInfo)
{
	CurrentWidget->SetIsEnabled(true);
}

void ASMLobbyController::OnReceiveRoomListUpdateResponse(bool Success, const TArray<FGrpcLobbyRoom>& RoomList)
{
	CurrentWidget->SetIsEnabled(true);
}
