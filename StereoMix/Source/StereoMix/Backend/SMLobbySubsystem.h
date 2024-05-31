// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGrpcClientSubsystem.h"
#include "SLobby/LobbyService.h"
#include "SMLobbySubsystem.generated.h"

/**
 * StereoMix Lobby Subsystem
 */
UCLASS(Abstract)
class STEREOMIX_API USMLobbySubsystem : public USMGrpcClientSubsystem
{
	GENERATED_BODY()

public:
	USMLobbySubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	ULobbyService* GetLobbyService() const { return Cast<ULobbyService>(GrpcService); }
};
