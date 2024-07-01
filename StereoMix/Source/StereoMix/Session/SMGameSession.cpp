// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameSession.h"

#include "StereoMixLog.h"

DEFINE_LOG_CATEGORY(LogSMGameSession)

// Sets default values
ASMGameSession::ASMGameSession()
{
	PrimaryActorTick.bCanEverTick = true;
	SessionName = TEXT("StereoMix GameSession");
	MaxPlayers = 6;

#if WITH_EDITOR
	bCanEnterRoom = true;
#else
	bCanEnterRoom = false;
#endif
}

bool ASMGameSession::IsConnectedWithBackend() const
{
	if (!RoomSubsystem)
	{
		return false;
	}

	return RoomSubsystem->IsConnectedWithBackend();
}

void ASMGameSession::UpdateRoomState(const EGrpcLobbyRoomState NewState)
{
	if (!IsConnectedWithBackend())
	{
		return;
	}

	FGrpcLobbyUpdateRoomStateRequest Request;
	Request.State = NewState;
	RoomSubsystem->GetLobbyService()->CallUpdateRoomState(
		Request,
		[this](const FGrpcResult& Result, const FGrpcLobbyUpdateRoomStateResponse& Response) {
			if (Result.Code != EGrpcResultCode::Ok)
			{
				UE_LOG(LogStereoMix, Error, TEXT("[SMGameSession] Room state update failed: %s, %s"), *Result.GetCodeString(), *Result.GetMessageString())
				return;
			}
			UE_LOG(LogStereoMix, Warning, TEXT("[SMGameSession] Room state updated: %s"), *UEnum::GetValueAsString(Response.UpdatedState))
		},
		RoomSubsystem->GetAuthentication());
}

void ASMGameSession::DeleteRoom()
{
	if (!IsConnectedWithBackend())
	{
		return;
	}

	RoomSubsystem->GetLobbyService()->CallDeleteRoom(
		FGrpcLobbyDeleteRoomRequest(),
		[this](const FGrpcResult& Result, const FGrpcLobbyDeleteRoomResponse&) {
			if (Result.Code == EGrpcResultCode::Ok)
			{
				UE_LOG(LogStereoMix, Warning, TEXT("[SMGameSession] Room deleted"))
			}
			else
			{
				UE_LOG(LogStereoMix, Error, TEXT("[SMGameSession] Room delete failed: %s, %s"), *Result.GetCodeString(), *Result.GetMessageString())
			}
		},
		RoomSubsystem->GetAuthentication());
}

void ASMGameSession::SetCanEnterRoom(const bool bCanEnter)
{
	bCanEnterRoom = bCanEnter;
}

void ASMGameSession::InitOptions(const FString& Options)
{
	// BUG: MaxPlayers가 생성자가 아니라 실제 여기서 적용됨
	MaxPlayers = 6;
	Super::InitOptions(Options);
	UE_LOG(LogSMGameSession, Warning, TEXT("InitOptions: %s"), *Options)
}

void ASMGameSession::RegisterServer()
{
	Super::RegisterServer();
	RoomSubsystem = GetGameInstance()->GetSubsystem<USMServerRoomSubsystem>();
	if (!RoomSubsystem)
	{
		UE_LOG(LogStereoMix, Warning, TEXT("RoomSubsystem is invalid. Room system disabled."))
	}
}

FString ASMGameSession::ApproveLogin(const FString& Options)
{
	UE_LOG(LogSMGameSession, Verbose, TEXT("ApproveLogin: %s"), *Options)

#if !WITH_EDITOR
	if (!bCanEnterRoom)
	{
		return TEXT("Room can not join.");
	}
#endif

	return Super::ApproveLogin(Options);
}
