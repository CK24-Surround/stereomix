// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SMGameplayAbility.h"
#include "SMGA_Neutralize.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMGA_Neutralize : public USMGameplayAbility
{
	GENERATED_BODY()

public:
	USMGA_Neutralize();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UFUNCTION()
	void OnNeutralizeTimeEnded();

	void WaitUntilBuzzerBeaterEnd();

	UFUNCTION()
	void OnBuzzerBeaterEnded(FGameplayEventData Payload);

	void HoldedStateExit();

	void NeutralizeExitSyncPoint();

	UFUNCTION()
	void NeutralizeExit();

	/** 잡고 있던 대상이 있다면 놓아줍니다. */
	void Release();

	UPROPERTY()
	TObjectPtr<UAnimMontage> CachedNeutralizeMontage;

	FGameplayTagContainer NoiseBreakedTags;

	UPROPERTY(EditAnywhere, Category = "Design")
	float NeutralizedTime = 6.5f;

	UPROPERTY(EditAnywhere, Category = "GAS|GE")
	TArray<TSubclassOf<UGameplayEffect>> StunEndedGEs;
};
