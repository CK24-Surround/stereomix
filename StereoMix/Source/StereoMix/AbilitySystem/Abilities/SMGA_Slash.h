// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGameplayAbility.h"
#include "SMGA_Slash.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMGA_Slash : public USMGameplayAbility
{
	GENERATED_BODY()

public:
	USMGA_Slash();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	void FocusToCursor();

	UFUNCTION()
	void SlashEnded();

	uint32 bCanInput:1 = false;
};
