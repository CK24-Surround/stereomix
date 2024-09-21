// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SMGameplayAbility.h"
#include "SMGA_Neutralize.generated.h"

class UAbilityTask_WaitDelay;
class UAbilityTask_WaitGameplayEvent;

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
	void OnMinimalNeutralizeTimeEnded();

	UFUNCTION()
	void OnMinimalNeutralizeTimerReset(FGameplayEventData Payload);

	UFUNCTION()
	void OnNeutralizeTimeEnded();

	void WaitUntilBuzzerBeaterEnd();

	UFUNCTION()
	void OnBuzzerBeaterEnded(FGameplayEventData Payload);

	void PrepareNeutralizeEnd();

	void NeutralizeExitSyncPoint();

	UFUNCTION()
	void NeutralizeEnd();

	UPROPERTY()
	TObjectPtr<UAnimMontage> CachedNeutralizeMontage;

	FGameplayTagContainer NoiseBreakedTags;

	UPROPERTY(EditAnywhere, Category = "Design")
	float NeutralizedTime = 6.5f;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitDelay> WaitTask;

	UPROPERTY(EditAnywhere, Category = "Design")
	float MinimalNeutralizedTime = 3.0f;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitDelay> MinimalWaitTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> WaitNoiseBreakEndWaitTask;

	UPROPERTY(EditAnywhere, Category = "GAS|GE")
	TArray<TSubclassOf<UGameplayEffect>> StunEndedGEs;
};
