// Copyright Surround, Inc. All Rights Reserved.


#include "SMClientLobbySubsystem.h"

#include "SMClientAuthSubsystem.h"
#include "SMUserAccount.h"
#include "StereoMix.h"
#include "StereoMixLog.h"

void USMClientLobbySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	ClientAuthService = Collection.InitializeDependency<USMClientAuthSubsystem>();

	LobbyClient = GetLobbyService()->MakeClient();
	LobbyClient->OnCreateRoomResponse.AddDynamic(this, &USMClientLobbySubsystem::OnGrpcCreateRoomResponse);
	LobbyClient->OnQuickMatchResponse.AddDynamic(this, &USMClientLobbySubsystem::OnGrpcQuickMatchResponse);
	LobbyClient->OnJoinRoomResponse.AddDynamic(this, &USMClientLobbySubsystem::OnGrpcJoinRoomResponse);
	LobbyClient->OnJoinRoomWithCodeResponse.AddDynamic(this, &USMClientLobbySubsystem::OnGrpcJoinRoomWithCodeResponse);
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

	const FGrpcMetaData MetaData = ClientAuthService->GetUserAccount()->GetAuthorizationHeader();
	LobbyClient->CreateRoom(GrpcContextHandle, Request, MetaData);
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

	const FGrpcMetaData MetaData = ClientAuthService->GetUserAccount()->GetAuthorizationHeader();
	LobbyClient->QuickMatch(GrpcContextHandle, Request, MetaData);
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

	const FGrpcMetaData MetaData = ClientAuthService->GetUserAccount()->GetAuthorizationHeader();
	LobbyClient->JoinRoom(GrpcContextHandle, Request, MetaData);
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
	GrpcContextHandle = LobbyClient->InitJoinRoomWithCode();

	const FGrpcMetaData MetaData = ClientAuthService->GetUserAccount()->GetAuthorizationHeader();
	LobbyClient->JoinRoomWithCode(GrpcContextHandle, Request, MetaData);
	return true;
}

void USMClientLobbySubsystem::Cancel() const
{
	UE_LOG(LogStereoMix, Warning, TEXT("[SMClientLobbySubsystem] Cancel requested"));
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

	ECreateRoomResult Result;
	FString ServerUrl;
	switch (GrpcResult.Code)
	{
	case EGrpcResultCode::Ok:
		Result = ECreateRoomResult::Success;
		ServerUrl = GetServerUrl(Response.Connection);
		break;

	case EGrpcResultCode::Cancelled:
		Result = ECreateRoomResult::Cancelled;
		break;

	case EGrpcResultCode::Internal:
		Result = ECreateRoomResult::InternalError;
		break;

	case EGrpcResultCode::DeadlineExceeded:
		Result = ECreateRoomResult::DeadlineExceeded;
		break;

	case EGrpcResultCode::Unauthenticated:
		Result = ECreateRoomResult::Unauthenticated;
		break;

	case EGrpcResultCode::InvalidArgument:
		Result = ECreateRoomResult::InvalidArgument;
		break;

	case EGrpcResultCode::ConnectionFailed:
		Result = ECreateRoomResult::ConnectionError;
		break;

	default:
		Result = ECreateRoomResult::UnknownError;
		break;
	}
	OnCreateRoomResponse.Broadcast(Result, ServerUrl);
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

	if (!OnQuickMatchResponse.IsBound())
	{
		return;
	}

	EQuickMatchResult Result;
	FString ServerUrl;
	switch (GrpcResult.Code)
	{
	case EGrpcResultCode::Ok:
		Result = EQuickMatchResult::Success;
		ServerUrl = GetServerUrl(Response.Connection);
		break;

	case EGrpcResultCode::Cancelled:
		Result = EQuickMatchResult::Cancelled;
		break;

	case EGrpcResultCode::Internal:
		Result = EQuickMatchResult::InternalError;
		break;

	case EGrpcResultCode::DeadlineExceeded:
		Result = EQuickMatchResult::DeadlineExceeded;
		break;

	case EGrpcResultCode::Unauthenticated:
		Result = EQuickMatchResult::Unauthenticated;
		break;

	case EGrpcResultCode::InvalidArgument:
		Result = EQuickMatchResult::InvalidArgument;
		break;

	case EGrpcResultCode::ConnectionFailed:
		Result = EQuickMatchResult::ConnectionError;
		break;

	default:
		Result = EQuickMatchResult::UnknownError;
		break;
	}
	OnQuickMatchResponse.Broadcast(Result, ServerUrl);
}

void USMClientLobbySubsystem::OnGrpcJoinRoomResponse(FGrpcContextHandle Handle, const FGrpcResult& GrpcResult, const FGrpcLobbyJoinRoomResponse& Response)
{
	if (Handle != GrpcContextHandle)
	{
		return;
	}

	if (GrpcResult.Code == EGrpcResultCode::Ok)
	{
		UE_LOG(LogStereoMix, Log, TEXT("[SMClientLobbySubsystem] join room success."))
	}
	else
	{
		UE_LOG(LogStereoMix, Error, TEXT("[SMClientLobbySubsystem] join room failed: %s, %s"), *GrpcResult.GetCodeString(), *GrpcResult.GetMessageString())
	}

	if (!OnJoinRoomResponse.IsBound())
	{
		return;
	}

	EJoinRoomResult Result;
	FString ServerUrl;
	switch (GrpcResult.Code)
	{
	case EGrpcResultCode::Ok:
		Result = EJoinRoomResult::Success;
		ServerUrl = GetServerUrl(Response.Connection);
		break;

	case EGrpcResultCode::Unauthenticated:
		Result = EJoinRoomResult::Unauthenticated;
		break;

	case EGrpcResultCode::InvalidArgument:
		Result = EJoinRoomResult::InvalidArgument;
		break;

	case EGrpcResultCode::NotFound:
		Result = EJoinRoomResult::RoomNotFound;
		break;

	case EGrpcResultCode::PermissionDenied:
		Result = EJoinRoomResult::InvalidPassword;
		break;

	case EGrpcResultCode::Aborted:
		Result = EJoinRoomResult::RoomFull;
		break;

	case EGrpcResultCode::Internal:
		Result = EJoinRoomResult::InternalError;
		break;

	case EGrpcResultCode::DeadlineExceeded:
		Result = EJoinRoomResult::DeadlineExceeded;
		break;

	case EGrpcResultCode::ConnectionFailed:
		Result = EJoinRoomResult::ConnectionError;
		break;

	default:
		Result = EJoinRoomResult::UnknownError;
		break;
	}
	OnJoinRoomResponse.Broadcast(Result, ServerUrl);
}

void USMClientLobbySubsystem::OnGrpcJoinRoomWithCodeResponse(FGrpcContextHandle Handle, const FGrpcResult& GrpcResult, const FGrpcLobbyJoinRoomWithCodeResponse& Response)
{
	if (Handle != GrpcContextHandle)
	{
		return;
	}

	if (GrpcResult.Code == EGrpcResultCode::Ok)
	{
		UE_LOG(LogStereoMix, Log, TEXT("[SMClientLobbySubsystem] join room with code success."))
	}
	else
	{
		UE_LOG(LogStereoMix, Error, TEXT("[SMClientLobbySubsystem] join room with code failed: %s, %s"), *GrpcResult.GetCodeString(), *GrpcResult.GetMessageString())
	}

	if (!OnJoinRoomWithCodeResponse.IsBound())
	{
		return;
	}

	EJoinRoomWithCodeResult Result;
	FString ServerUrl;
	switch (GrpcResult.Code)
	{
	case EGrpcResultCode::Ok:
		Result = EJoinRoomWithCodeResult::Success;
		ServerUrl = GetServerUrl(Response.Connection);
		break;

	case EGrpcResultCode::Unauthenticated:
		Result = EJoinRoomWithCodeResult::Unauthenticated;
		break;

	case EGrpcResultCode::InvalidArgument:
		Result = EJoinRoomWithCodeResult::InvalidArgument;
		break;

	case EGrpcResultCode::NotFound:
		Result = EJoinRoomWithCodeResult::RoomNotFound;
		break;

	case EGrpcResultCode::Aborted:
		Result = EJoinRoomWithCodeResult::RoomFull;
		break;

	case EGrpcResultCode::Internal:
		Result = EJoinRoomWithCodeResult::InternalError;
		break;

	case EGrpcResultCode::DeadlineExceeded:
		Result = EJoinRoomWithCodeResult::DeadlineExceeded;
		break;

	case EGrpcResultCode::ConnectionFailed:
		Result = EJoinRoomWithCodeResult::ConnectionError;
		break;

	default:
		Result = EJoinRoomWithCodeResult::UnknownError;
		break;
	}
	OnJoinRoomWithCodeResponse.Broadcast(Result, ServerUrl);
}

FString USMClientLobbySubsystem::GetServerUrl(const FGrpcLobbyRoomConnectionInfo& ConnectionInfo)
{
	return FString::Printf(TEXT("%s:%d"), *ConnectionInfo.Host, ConnectionInfo.Port);
}
