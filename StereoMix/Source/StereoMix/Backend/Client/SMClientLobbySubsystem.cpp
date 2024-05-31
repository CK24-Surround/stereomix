// Copyright Surround, Inc. All Rights Reserved.


#include "SMClientLobbySubsystem.h"

#include "SMClientAuthSubsystem.h"
#include "SMUserAccount.h"
#include "StereoMix.h"

void USMClientLobbySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	ClientAuthService = Collection.InitializeDependency<USMClientAuthSubsystem>();

	LobbyClient = GetLobbyService()->MakeClient();
	LobbyClient->OnCreateRoomResponse.AddDynamic(this, &USMClientLobbySubsystem::OnGrpcCreateRoomResponse);
	LobbyClient->OnQuickMatchResponse.AddDynamic(this, &USMClientLobbySubsystem::OnGrpcQuickMatchResponse);
	LobbyClient->OnJoinRoomResponse.AddDynamic(this, &USMClientLobbySubsystem::OnGrpcJoinRoomResponse);
}

bool USMClientLobbySubsystem::CreateRoom(const FString& RoomName, FGrpcLobbyRoomConfig RoomConfig, const FString& Password)
{
	if (!LobbyClient || !IsAuthenticated() || IsBusy())
	{
		return false;
	}

	FGrpcLobbyCreateRoomRequest Request;
	Request.RoomName = RoomName;
	Request.Password = Password;
	Request.Config = RoomConfig;
	GrpcContextHandle = LobbyClient->InitCreateRoom();
	LobbyClient->CreateRoom(GrpcContextHandle, Request, ClientAuthService->GetUserAccount()->GetAuthorizationHeader());
	return true;
}

bool USMClientLobbySubsystem::QuickMatch()
{
	if (!LobbyClient || !IsAuthenticated() || IsBusy())
	{
		return false;
	}

	FGrpcLobbyQuickMatchRequest Request;
	GrpcContextHandle = LobbyClient->InitQuickMatch();
	LobbyClient->QuickMatch(GrpcContextHandle, Request, ClientAuthService->GetUserAccount()->GetAuthorizationHeader());
	return true;
}

bool USMClientLobbySubsystem::JoinRoom(const FString& RoomId, const FString& Password)
{
	if (!LobbyClient || !IsAuthenticated() || IsBusy())
	{
		return false;
	}

	FGrpcLobbyJoinRoomRequest Request;
	Request.RoomId = RoomId;
	Request.Password = Password;
	GrpcContextHandle = LobbyClient->InitJoinRoom();
	LobbyClient->JoinRoom(GrpcContextHandle, Request, ClientAuthService->GetUserAccount()->GetAuthorizationHeader());
	return true;
}

bool USMClientLobbySubsystem::JoinRoomWithCode(const FString& RoomCode)
{
	if (!LobbyClient || !IsAuthenticated() || IsBusy())
	{
		return false;
	}

	FGrpcLobbyJoinRoomWithCodeRequest Request;
	Request.RoomCode = RoomCode;
	GrpcContextHandle = LobbyClient->InitJoinRoom();
	LobbyClient->JoinRoomWithCode(GrpcContextHandle, Request, ClientAuthService->GetUserAccount()->GetAuthorizationHeader());
	return true;
}

void USMClientLobbySubsystem::Cancel() const
{
	LobbyClient->TryCancel(GrpcContextHandle);
}

bool USMClientLobbySubsystem::IsBusy() const
{
	bool bBusy = LobbyClient->GetContextState(GrpcContextHandle) == EGrpcContextState::Busy;
	if (bBusy)
	{
		UE_LOG(LogStereoMix, Warning, TEXT("[SMClientLobbySubsystem] 다른 요청이 이미 진행 중입니다."))
	}
	return bBusy;
}

bool USMClientLobbySubsystem::IsAuthenticated() const
{
	return ClientAuthService && ClientAuthService->IsAuthenticated();
}

void USMClientLobbySubsystem::OnGrpcCreateRoomResponse(FGrpcContextHandle Handle, const FGrpcResult& GrpcResult, const FGrpcLobbyCreateRoomResponse& Response)
{
	if (Handle != GrpcContextHandle)
	{
		return;
	}

	if (GrpcResult.Code == EGrpcResultCode::Ok)
	{
		UE_LOG(LogStereoMix, Log, TEXT("[SMClientLobbySubsystem] create room success."))
	}
	else
	{
		UE_LOG(LogStereoMix, Error, TEXT("[SMClientLobbySubsystem] create room failed: %s, %s"), *GrpcResult.GetCodeString(), *GrpcResult.GetMessageString())
	}

	if (!OnCreateRoomResponse.IsBound())
	{
		return;
	}

	switch (GrpcResult.Code)
	{
	case EGrpcResultCode::Ok:
		OnCreateRoomResponse.Broadcast(ECreateRoomResult::Success, GetServerUrl(Response.Connection));
		break;
		
	case EGrpcResultCode::Cancelled:
		OnCreateRoomResponse.Broadcast(ECreateRoomResult::Cancelled, TEXT(""));
		break;
		
	case EGrpcResultCode::InvalidArgument:
		OnCreateRoomResponse.Broadcast(ECreateRoomResult::InvalidArgument, TEXT(""));
		break;
		
	case EGrpcResultCode::DeadlineExceeded:
		OnCreateRoomResponse.Broadcast(ECreateRoomResult::DeadlineExceeded, TEXT(""));
		break;
		
	case EGrpcResultCode::Internal:
		OnCreateRoomResponse.Broadcast(ECreateRoomResult::InternalError, TEXT(""));
		break;
		
	case EGrpcResultCode::Unauthenticated:
		OnCreateRoomResponse.Broadcast(ECreateRoomResult::Unauthenticated, TEXT(""));
		break;
		
	case EGrpcResultCode::ConnectionFailed:
		OnCreateRoomResponse.Broadcast(ECreateRoomResult::ConnectionError, TEXT(""));
		break;

	case EGrpcResultCode::Unknown:
	default:
		OnCreateRoomResponse.Broadcast(ECreateRoomResult::UnknownError, TEXT(""));
		break;
	}
}

void USMClientLobbySubsystem::OnGrpcQuickMatchResponse(FGrpcContextHandle Handle, const FGrpcResult& GrpcResult, const FGrpcLobbyQuickMatchResponse& Response)
{
	if (Handle != GrpcContextHandle)
	{
		return;
	}

	if (GrpcResult.Code == EGrpcResultCode::Ok)
	{
		UE_LOG(LogStereoMix, Log, TEXT("[SMClientLobbySubsystem] quick match success."))
	}
	else
	{
		UE_LOG(LogStereoMix, Error, TEXT("[SMClientLobbySubsystem] quick match failed: %s, %s"), *GrpcResult.GetCodeString(), *GrpcResult.GetMessageString())
	}

	if (!OnCreateRoomResponse.IsBound())
	{
		return;
	}

	switch (GrpcResult)
	{
	default:
		break;;
	}
}

void USMClientLobbySubsystem::OnGrpcJoinRoomResponse(FGrpcContextHandle Handle, const FGrpcResult& GrpcResult, const FGrpcLobbyJoinRoomResponse& Response)
{
}

FString USMClientLobbySubsystem::GetServerUrl(const FGrpcLobbyRoomConnectionInfo& ConnectionInfo)
{
	return FString::Printf(TEXT("%s:%d"), *ConnectionInfo.Host, ConnectionInfo.Port);
}
