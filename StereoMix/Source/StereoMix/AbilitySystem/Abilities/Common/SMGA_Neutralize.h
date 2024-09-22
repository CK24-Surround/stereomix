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

	FGameplayTagContainer NoiseBreakedTags;

	/** 무력화 기본 시간입니다. */
	UPROPERTY(EditAnywhere, Category = "Design")
	float NeutralizedTime = 6.5f;

	/** 무력화 기본 타이머 태스크입니다. */
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitDelay> WaitTask;

	/** 무력화 최소 시간입니다. 노이즈 브레이크 액션을 당하지 않는 경우 이 시간이 적용됩니다. */
	UPROPERTY(EditAnywhere, Category = "Design")
	float MinimalNeutralizedTime = 3.0f;

	/** 무력화 최소 타이머 태스크입니다. */
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitDelay> MinimalWaitTask;

	/** 무력화 최소 타이머를 리셋하기 위한 노이즈 브레이크 감지용 태스크입니다. */
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> WaitNoiseBreakEndWaitTask;

	UPROPERTY(EditAnywhere, Category = "GAS|GE")
	TArray<TSubclassOf<UGameplayEffect>> StunEndedGEs;
};
