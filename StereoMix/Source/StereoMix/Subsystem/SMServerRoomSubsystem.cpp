// Copyright Surround, Inc. All Rights Reserved.


#include "SMServerRoomSubsystem.h"

#include "StereoMixLog.h"
#include "TurboLinkGrpcManager.h"
#include "GameInstance/SMGameInstance.h"

bool USMServerRoomSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	const USMGameInstance* GameInstance = Cast<USMGameInstance>(Outer);

#if UE_EDITOR
	return false;
#else
	return GameInstance && !GameInstance->IsCustomGame();
#endif
}

void USMServerRoomSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

#if WITH_SERVER_CODE
	AuthToken = FPlatformMisc::GetEnvironmentVariable(TEXT("STEREOMIX_AUTH_TOKEN"));

	if (AuthToken.IsEmpty())
	{
		UE_LOG(LogStereoMix, Error, TEXT("[RoomSubsystem] Failed to find Server auth token. Room system will disabled."))
		return;
	}
	
	RoomId = FPlatformMisc::GetEnvironmentVariable(TEXT("STEREOMIX_ROOM_ID"));
	RoomCode = FPlatformMisc::GetEnvironmentVariable(TEXT("STEREOMIX_ROOM_CODE"));
	AuthorizationHeader.MetaData.Add(TEXT("authorization"), TEXT("Bearer ") + AuthToken);

	UE_LOG(LogStereoMix, Log, TEXT("[RoomSubsystem] STEREOMIX_ROOM_ID: %s"), *RoomId)
	UE_LOG(LogStereoMix, Log, TEXT("[RoomSubsystem] STEREOMIX_ROOM_CODE: %s"), *RoomCode)
	UE_LOG(LogStereoMix, Log, TEXT("[RoomSubsystem] STEREOMIX_AUTH_TOKEN: %s"), *AuthToken)
	UE_LOG(LogStereoMix, Log, TEXT("[RoomSubsystem] Authentication: %s"), *AuthorizationHeader.MetaData.FindRef(TEXT("authorization")))

	UTurboLinkGrpcManager* GrpcManager = GetGameInstance()->GetSubsystem<UTurboLinkGrpcManager>();
	LobbyService = CastChecked<ULobbyService>(GrpcManager->MakeService("LobbyService"));
	LobbyService->Connect();
	LobbyServiceClient = LobbyService->MakeClient();
#endif
}

void USMServerRoomSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

bool USMServerRoomSubsystem::IsConnectedWithBackend()
{
	if (GetWorld() && GetWorld()->GetNetMode() != NM_DedicatedServer)
	{
		return false;
	}

	if (!LobbyService || !LobbyServiceClient)
	{
		return false;
	}

	if (LobbyService->GetServiceState() != EGrpcServiceState::Idle && LobbyService->GetServiceState() != EGrpcServiceState::Ready)
	{
		return false;
	}
	
	return true;
}
