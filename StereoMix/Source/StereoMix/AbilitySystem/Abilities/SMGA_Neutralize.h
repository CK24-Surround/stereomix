// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGameplayAbility.h"
#include "SMGA_Neutralize.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMGA_Neutralize : public USMGameplayAbility
{
	GENERATED_BODY()

public:
	USMGA_Neutralize();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	/** 잡고 있던 대상이 잇다면 놓아줍니다. */
	void Release();

	void OnNeutralizeEnd();

	void ResetNeutralizeState();

	UPROPERTY()
	TObjectPtr<UAnimMontage> CachedNeutralizeMontage;

	FGameplayTagContainer NoiseBreakedTags;

	UPROPERTY(EditAnywhere, Category = "Design")
	float NeutralizedTime = 6.5f;
};
