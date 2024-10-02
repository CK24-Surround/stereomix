// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SMGameplayAbility.h"

#include "SMGA_Immune.generated.h"

class UFMODEvent;
/**
 *
 */
UCLASS(Abstract)
class STEREOMIX_API USMGA_Immune : public USMGameplayAbility
{
	GENERATED_BODY()

public:
	USMGA_Immune();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void OnImmuneEnd();

	UPROPERTY(EditAnywhere, Category = "Design|Immune")
	float MoveSpeedMultiplier = 1.5f;

	UPROPERTY(EditAnywhere, Category = "Design|Immune")
	float ImmuneTime = 1.0f;
};
