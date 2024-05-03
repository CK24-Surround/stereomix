// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGameplayAbility_TileTrigger.h"
#include "SMGameplayAbility_TileTrigger_Smash.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMGameplayAbility_TileTrigger_Smash : public USMGameplayAbility_TileTrigger
{
	GENERATED_BODY()

protected:
	virtual void TileTrigger() override;

	void ProcessContinuousTileTrigger();

protected:
	UPROPERTY(EditAnywhere, Category = "Smash|TileTirgger", DisplayName = "타일이 모두 트리거 되는데 걸리는 시간")
	float TotalTriggerTime = 0.267f;

	int32 CurrentTriggerCount = 0;
	int32 MaxTriggerCount = 0;
	float Range = 1.0f;
	float DelayTime = 0.0f;
};
