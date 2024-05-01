// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGameplayAbility.h"
#include "SMGameplayAbility_Immune.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMGameplayAbility_Immune : public USMGameplayAbility
{
	GENERATED_BODY()

public:
	USMGameplayAbility_Immune();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	
};
