// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StereoMixGameplayAbility.h"
#include "StereoMixGameplayAbility_Stun.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API UStereoMixGameplayAbility_Stun : public UStereoMixGameplayAbility
{
	GENERATED_BODY()

public:
	UStereoMixGameplayAbility_Stun();

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION()
	void OnStunTimeEnded();

	void ResetSmashedState();

protected:
	/** 잡힌 상태에서 벗어납니다. */
	void ResetCaughtState();

	/** 타겟 캐릭터로부터 소스 캐릭터를 디태치합니다. */
	void DetachFromTargetCharacter(AStereoMixPlayerCharacter* InTargetCharacter);

	/** 스턴 상태에서 벗어납니다. */
	void ResetStunState();

protected:
	UFUNCTION()
	void OnComplete();

	UFUNCTION()
	void OnInterrupted();

	/** 스턴이 종료될때 필요한 처리를 합니다.. */
	void OnStunEnded();

protected:
	UPROPERTY(EditAnywhere, Category = "Montage")
	TObjectPtr<UAnimMontage> StunMontage;

	UPROPERTY(EditAnywhere, Category = "Montage")
	TObjectPtr<UAnimMontage> CaughtExitMontage;

	UPROPERTY(EditAnywhere, Category = "Montage")
	TObjectPtr<UAnimMontage> StandUpMontage;

	UPROPERTY(EditAnywhere, Category = "GAS|GE")
	TArray<TSubclassOf<UGameplayEffect>> StunEndedGEs;

protected:
	UPROPERTY(EditAnywhere, Category = "Stun")
	float StunTime = 6.0f;
};
