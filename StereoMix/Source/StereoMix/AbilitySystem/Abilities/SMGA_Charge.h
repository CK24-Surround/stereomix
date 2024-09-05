// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGameplayAbility.h"
#include "SMGA_Charge.generated.h"

class UAbilityTask_ApplyRootMotionConstantForce;

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMGA_Charge : public USMGameplayAbility
{
	GENERATED_BODY()

public:
	USMGA_Charge();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	void OnChargeBlocked();

	UFUNCTION()
	void OnChargeEnded();

	FName OriginalCollisionProfileName;
};
