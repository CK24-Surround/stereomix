// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SMRoomMode.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API ASMRoomMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASMRoomMode();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
};
