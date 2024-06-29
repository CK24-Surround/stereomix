// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SMGameplayAbility_CaughtExit.h"

#include "SMGameplayAbility_CaughtExitOnStunEnd.generated.h"

/**
 *
 */
UCLASS()
class STEREOMIX_API USMGameplayAbility_CaughtExitOnStunEnd : public USMGameplayAbility_CaughtExit
{
	GENERATED_BODY()

public:
	USMGameplayAbility_CaughtExitOnStunEnd();

protected:
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
};
