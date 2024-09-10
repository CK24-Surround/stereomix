// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGA_Skill.h"
#include "SMGA_ImpactArrow.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMGA_ImpactArrow : public USMGA_Skill
{
	GENERATED_BODY()

public:
	USMGA_ImpactArrow();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	void Shot();

	UPROPERTY()
	TObjectPtr<UEnhancedInputComponent> InputComponent;
};
