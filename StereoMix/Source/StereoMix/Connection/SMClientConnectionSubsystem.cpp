// Copyright Surround, Inc. All Rights Reserved.


#include "SMClientConnectionSubsystem.h"

#include "StereoMix.h"
#include "TurboLinkGrpcManager.h"
#include "FunctionLibraries/SMGrpcServiceUtil.h"
#include "SAuth/AuthClient.h"
#include "SAuth/AuthService.h"

USMClientConnectionSubsystem::USMClientConnectionSubsystem()
{
}

bool USMClientConnectionSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
#if UE_SERVER
	return false;
#else
	return true;
#endif
}

void USMClientConnectionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UTurboLinkGrpcManager* TurboLinkManager = GetGameInstance()->GetSubsystem<UTurboLinkGrpcManager>();

	// Init AuthService
	AuthServiceClient->OnGuestLoginResponse.AddUniqueDynamic(this, &USMClientConnectionSubsystem::OnGuestLoginResponse);

	// Init LobbyService
	LobbyService = Cast<ULobbyService>(TurboLinkManager->MakeService(TEXT("LobbyService")));
	LobbyService->OnServiceStateChanged.AddUniqueDynamic(this, &USMClientConnectionSubsystem::OnLobbyServiceStateChanged);
	LobbyServiceClient = LobbyService->MakeClient();
	LobbyServiceClient->OnCreateRoomResponse.AddUniqueDynamic(this, &USMClientConnectionSubsystem::OnCreateRoomResponse);
	LobbyServiceClient->OnJoinRoomResponse.AddUniqueDynamic(this, &USMClientConnectionSubsystem::OnJoinRoomResponse);
}

void USMClientConnectionSubsystem::ConnectLobbyService() const
{
	check(LobbyService)
	LobbyService->Connect();
}

void USMClientConnectionSubsystem::GuestLogin(const FText& UserName)
{
	if (!USMGrpcServiceUtil::IsServiceReadyToCall(AuthService))
	{
		UE_LOG(LogStereoMix, Error, TEXT("gRPC AuthService is not ready to call."))
		LoginEvent.Broadcast(EGrpcResultCode::ConnectionFailed);
		return;
	}

	if (USMGrpcServiceUtil::IsBusy(AuthServiceClient, GuestLoginHandle))
	{
		UE_LOG(LogStereoMix, Error, TEXT("gRPC AuthService::GuestLogin is already in progress."))
		LoginEvent.Broadcast(EGrpcResultCode::Aborted);
		return;
	}

	GuestLoginHandle = AuthServiceClient->InitGuestLogin();
	FGrpcAuthGuestLoginRequest Request;
	Request.UserName = UserName.ToString();
	AuthServiceClient->GuestLogin(GuestLoginHandle, Request);

	Account.UserName = UserName;
}

void USMClientConnectionSubsystem::OnLobbyServiceStateChanged(EGrpcServiceState ServiceState)
{
	UE_LOG(LogStereoMix, Log, TEXT("gRPC LobbyService state changed: %s"), *UEnum::GetValueAsString(ServiceState))
	LobbyServiceStateChangedEvent.Broadcast(ServiceState);
}

void USMClientConnectionSubsystem::OnGuestLoginResponse(FGrpcContextHandle Handle, const FGrpcResult& Result, const FGrpcAuthLoginResponse& Response)
{
	if (Result.Code != EGrpcResultCode::Ok)
	{
		UE_LOG(LogStereoMix, Error, TEXT("gRPC AuthService::GuestLogin failed: %s, %s"), *Result.GetCodeString(), *Result.GetMessageString())
		LoginEvent.Broadcast(Result.Code);
		return;
	}

	UE_LOG(LogStereoMix, Log, TEXT("gRPC AuthService::GuestLogin succeeded."))
	AuthToken = Response.Token;
	bIsAuthenticated = true;
	LoginEvent.Broadcast(Result.Code);
}


void USMClientConnectionSubsystem::CreateRoom(const FString& RoomName, const EGrpcLobbyRoomVisibility Visibility, const EGrpcLobbyGameMode GameMode, EGrpcLobbyGameMap Map, const FString& Password)
{
	if (!USMGrpcServiceUtil::IsServiceReadyToCall(LobbyService))
	{
		UE_LOG(LogStereoMix, Error, TEXT("gRPC LobbyService is not ready to call."))
		CreateRoomEvent.Broadcast(EGrpcResultCode::ConnectionFailed, EGrpcLobbyRoomDeploymentStatus::ROOM_DEPLOYMENT_STATUS_UNSPECIFIED, FString());
		return;
	}

	if (USMGrpcServiceUtil::IsBusy(LobbyServiceClient, CreateRoomHandle))
	{
		UE_LOG(LogStereoMix, Error, TEXT("gRPC LobbyService::CreateRoom is already in progress."))
		CreateRoomEvent.Broadcast(EGrpcResultCode::Aborted, EGrpcLobbyRoomDeploymentStatus::ROOM_DEPLOYMENT_STATUS_UNSPECIFIED, FString());
		return;
	}

	if (Visibility == EGrpcLobbyRoomVisibility::ROOM_VISIBILITY_PRIVATE && Password.IsEmpty())
	{
		UE_LOG(LogStereoMix, Error, TEXT("Private room must have a password."))
		CreateRoomEvent.Broadcast(EGrpcResultCode::InvalidArgument, EGrpcLobbyRoomDeploymentStatus::ROOM_DEPLOYMENT_STATUS_UNSPECIFIED, FString());
		return;
	}

	CreateRoomHandle = LobbyServiceClient->InitCreateRoom();
	FGrpcLobbyCreateRoomRequest Request;
	Request.RoomName = RoomName;
	Request.Config.Visibility = Visibility;
	Request.Config.Mode = GameMode;
	Request.Config.Map = Map;
	Request.Password = Password;
	LobbyServiceClient->CreateRoom(CreateRoomHandle, Request, GetAuthorizationMetaData());
}

// ReSharper disable once CppMemberFunctionMayBeConst
void USMClientConnectionSubsystem::OnCreateRoomResponse(FGrpcContextHandle Handle, const FGrpcResult& Result, const FGrpcLobbyCreateRoomResponse& Response)
{
	FString ConnectionUrl;
	if (Result.Code != EGrpcResultCode::Ok || Response.DeployStatus == EGrpcLobbyRoomDeploymentStatus::ROOM_DEPLOYMENT_STATUS_ERROR)
	{
		UE_LOG(LogStereoMix, Error, TEXT("gRPC LobbyService::CreateRoom failed: %s, %s"), *Result.GetCodeString(), *Result.GetMessageString())
		CreateRoomEvent.Broadcast(Result.Code, Response.DeployStatus, FString());

		if (Result.Code == EGrpcResultCode::Unauthenticated)
		{
			bIsAuthenticated = false;
		}

		CreateRoomEvent.Broadcast(Result.Code, Response.DeployStatus, ConnectionUrl);
		return;
	}
	
	if (Response.DeployStatus == EGrpcLobbyRoomDeploymentStatus::ROOM_DEPLOYMENT_STATUS_READY)
	{
		const FGrpcLobbyRoomConnectionInfo& Connection = Response.Connection;
		ConnectionUrl = FString::Printf(TEXT("%s:%d"), *Connection.Host, Connection.Port);
		UE_LOG(LogStereoMix, Log, TEXT("gRPC LobbyService::CreateRoom succeeded. ConnectionUrl: %s"), *ConnectionUrl)
	}
	else
	{
		UE_LOG(LogStereoMix, Log, TEXT("gRPC LobbyService::CreateRoom is in progress. DeploymentStatus: %s"), *UEnum::GetValueAsString(Response.DeployStatus))
	}
	
	CreateRoomEvent.Broadcast(Result.Code, Response.DeployStatus, ConnectionUrl);
}

void USMClientConnectionSubsystem::JoinRoom(const FString& RoomId, const FString& Password)
{
	FGrpcLobbyJoinRoomRequest Request;
	Request.Id.IdCase = EGrpcLobbyJoinRoomRequestId::RoomId;
	Request.Id.RoomId = RoomId;
	Request.Password = Password;
	JoinRoomInternal(Request);
}

void USMClientConnectionSubsystem::JoinRoomUsingShortCode(const FString& ShortCode)
{
	FGrpcLobbyJoinRoomRequest Request;
	Request.Id.IdCase = EGrpcLobbyJoinRoomRequestId::ShortRoomId;
	Request.Id.ShortRoomId = ShortCode;
	JoinRoomInternal(Request);
}

void USMClientConnectionSubsystem::JoinRoomInternal(const FGrpcLobbyJoinRoomRequest& Request)
{
	if (!USMGrpcServiceUtil::IsServiceReadyToCall(LobbyService))
	{
		UE_LOG(LogStereoMix, Error, TEXT("gRPC LobbyService is not ready to call."))
		JoinRoomEvent.Broadcast(EGrpcResultCode::ConnectionFailed, FString());
		return;
	}

	if (USMGrpcServiceUtil::IsBusy(LobbyServiceClient, JoinRoomHandle))
	{
		UE_LOG(LogStereoMix, Error, TEXT("gRPC LobbyService::JoinRoom is already in progress."))
		JoinRoomEvent.Broadcast(EGrpcResultCode::Aborted, FString());
		return;
	}

	if (Request.Id.RoomId.IsEmpty() && Request.Id.ShortRoomId.IsEmpty())
	{
		UE_LOG(LogStereoMix, Error, TEXT("RoomId or ShortCode must be specified."))
		JoinRoomEvent.Broadcast(EGrpcResultCode::InvalidArgument, FString());
		return;
	}

	JoinRoomHandle = LobbyServiceClient->InitJoinRoom();
	LobbyServiceClient->JoinRoom(JoinRoomHandle, Request, GetAuthorizationMetaData());
}

// ReSharper disable once CppMemberFunctionMayBeConst
void USMClientConnectionSubsystem::OnJoinRoomResponse(FGrpcContextHandle Handle, const FGrpcResult& Result, const FGrpcLobbyJoinRoomResponse& Response)
{
	if (Result.Code != EGrpcResultCode::Ok)
	{
		if (Result.Code == EGrpcResultCode::Unauthenticated)
		{
			bIsAuthenticated = false;
		}

		UE_LOG(LogStereoMix, Error, TEXT("gRPC LobbyService::JoinRoom failed: %s, %s"), *Result.GetCodeString(), *Result.GetMessageString())
		JoinRoomEvent.Broadcast(Result.Code, FString());

		return;
	}

	const FGrpcLobbyRoomConnectionInfo& Connection = Response.Connection;
	const FString ConnectionUrl = FString::Printf(TEXT("%s:%d"), *Connection.Host, Connection.Port);
	JoinRoomEvent.Broadcast(Result.Code, ConnectionUrl);
	UE_LOG(LogStereoMix, Log, TEXT("gRPC LobbyService::JoinRoom succeeded. ConnectionUrl: %s"), *ConnectionUrl)
}
