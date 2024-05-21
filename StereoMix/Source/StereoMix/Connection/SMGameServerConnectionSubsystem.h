// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMConnectionSubsystem.h"
#include "SMGameServerConnectionSubsystem.generated.h"

/**
 * StereoMix Game Server Connection Subsystem
 */
UCLASS(Config=Game, DefaultConfig)
class STEREOMIX_API USMGameServerConnectionSubsystem : public USMConnectionSubsystem
{
	GENERATED_BODY()

public:
	FString ShortRoomCode;
	
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
};
