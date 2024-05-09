// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGameplayAbility.h"
#include "SMGameplayAbility_CatchBuff.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMGameplayAbility_CatchBuff : public USMGameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION()
	void OnCatchRelease();

protected:
	UPROPERTY(EditAnywhere, Category = "CatchBuff")
	float MoveSpeedMultiply = 1.5f;
};
