// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGameplayAbility.h"
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
	void OnNeutralizeTimeEnd();

	void WaitUntilBuzzerBeaterEnd();

	UFUNCTION()
	void OnBuzzerBeaterEnded(FGameplayEventData Payload);

	void HoldedStateExit();

	void NeutralizeExit();

	UFUNCTION()
	void NeutralizeExitMontageEnded();

	/** 잡고 있던 대상이 있다면 놓아줍니다. */
	void Release();

	/** 무력화 종료 이펙트를 실행합니다. */
	void NewtralizeEndEffect();

	UPROPERTY()
	TObjectPtr<UAnimMontage> CachedNeutralizeMontage;

	FGameplayTagContainer NoiseBreakedTags;

	UPROPERTY(EditAnywhere, Category = "Design")
	float NeutralizedTime = 6.5f;

	UPROPERTY(EditAnywhere, Category = "GAS|GE")
	TArray<TSubclassOf<UGameplayEffect>> StunEndedGEs;
};
