// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMPlayerState.h"
#include "SMCharacterSelectPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API ASMCharacterSelectPlayerState : public ASMPlayerState
{
	GENERATED_BODY()

public:
	ASMCharacterSelectPlayerState();
	virtual void BeginPlay() override;
};
