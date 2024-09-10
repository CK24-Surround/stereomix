// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SMGameplayAbility.h"
#include "SMGA_StaminaControl.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMGA_StaminaControl : public USMGameplayAbility
{
	GENERATED_BODY()

public:
	USMGA_StaminaControl();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	FActiveGameplayEffectHandle StaminaGEHandle;
};
