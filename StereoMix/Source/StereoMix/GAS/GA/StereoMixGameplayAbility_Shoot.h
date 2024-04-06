// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "StereoMixGameplayAbility_Shoot.generated.h"

/**
 * 이 GA를 사용하려면 UStereoMixCharacterAttributeSet을 가지고 있어야합니다.
 */
UCLASS()
class STEREOMIX_API UStereoMixGameplayAbility_Shoot : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UStereoMixGameplayAbility_Shoot();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UFUNCTION()
	void OnFinished();

protected:
	UPROPERTY()
	TObjectPtr<UAnimMontage> ShootMontage;
};
