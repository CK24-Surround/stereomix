// Copyright Surround, Inc. All Rights Reserved.


#include "SMServerRoomSubsystem.h"

#include "StereoMixLog.h"
#include "TurboLinkGrpcManager.h"
#include "GameInstance/SMGameInstance.h"

bool USMServerRoomSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	const USMGameInstance* GameInstance = Cast<USMGameInstance>(Outer);
	return GameInstance && !GameInstance->IsCustomGame();
}

void USMServerRoomSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	#if WITH_SERVER_CODE
	AuthToken = FPlatformMisc::GetEnvironmentVariable(TEXT("STEREOMIX_AUTH_TOKEN"));
	RoomId = FPlatformMisc::GetEnvironmentVariable(TEXT("STEREOMIX_ROOM_ID"));
	RoomCode = FPlatformMisc::GetEnvironmentVariable(TEXT("STEREOMIX_ROOM_CODE"));
	AuthorizationHeader.MetaData.Add(TEXT("authorization"), TEXT("Bearer ") + AuthToken);
	
	UE_LOG(LogStereoMix, Log, TEXT("[RoomSubsystem] STEREOMIX_ROOM_ID: %s"), *RoomId)
	UE_LOG(LogStereoMix, Log, TEXT("[RoomSubsystem] STEREOMIX_ROOM_CODE: %s"), *RoomCode)
	UE_LOG(LogStereoMix, Log, TEXT("[RoomSubsystem] STEREOMIX_AUTH_TOKEN: %s"), *AuthToken)
	UE_LOG(LogStereoMix, Log, TEXT("[RoomSubsystem] Authentication: %s"), *AuthorizationHeader.MetaData.FindRef(TEXT("authorization")))

	UTurboLinkGrpcManager* GrpcManager = GetGameInstance()->GetSubsystem<UTurboLinkGrpcManager>();
	LobbyService = CastChecked<ULobbyService>(GrpcManager->MakeService("LobbyService"));
	#endif
}

void USMServerRoomSubsystem::Deinitialize()
{
	Super::Deinitialize();
}