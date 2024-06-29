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

public:
	USMGameplayAbility_CatchBuff();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(EditAnywhere, Category = "CatchBuff")
	float MoveSpeedMultiply = 1.5f;
};
