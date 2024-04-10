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

protected:
	/** 스매시 종료 후 호출됩니다. */
	UFUNCTION()
	void OnSmashEnded(FGameplayEventData Payload);

protected:
	/** 잡기에서 탈출합니다. */
	void CaughtExit();

	/** 타겟 캐릭터로부터 소스 캐릭터를 디태치합니다. */
	void DetachToTargetCharacter();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCSetRotationWithTarget(float InYaw);

protected:
	/** 스턴에서 탈출합니다. */
	void StunExit();

protected:
	UFUNCTION()
	void OnComplete();

	UFUNCTION()
	void OnInterrupted();

	/** 스턴이 종료될때 필요한 처리를 합니다.. */
	void OnStunEnded();
};
