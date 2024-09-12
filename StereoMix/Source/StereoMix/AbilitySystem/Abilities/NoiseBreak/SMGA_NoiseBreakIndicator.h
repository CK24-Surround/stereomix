// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SMGameplayAbility.h"
#include "SMGA_NoiseBreakIndicator.generated.h"

class USMAT_SkillIndicator;

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMGA_NoiseBreakIndicator : public USMGameplayAbility
{
	GENERATED_BODY()

public:
	USMGA_NoiseBreakIndicator();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	TWeakObjectPtr<USMAT_SkillIndicator> SkillIndicatorTask;
};
