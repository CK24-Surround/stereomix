// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StereoMixGameplayAbility.h"
#include "StereoMixGameplayAbility_Smash.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API UStereoMixGameplayAbility_Smash : public UStereoMixGameplayAbility
{
	GENERATED_BODY()

public:
	UStereoMixGameplayAbility_Smash();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	FGameplayTag SmashNotifyEventTag;
	FGameplayTag SmashedStateTag;
	FGameplayTag SmashedAbilityTag;
	FGameplayTag CatchStateTag;
	FGameplayTag CaughtStateTag;

protected:
	UFUNCTION()
	void OnCompleted();

	UFUNCTION()
	void OnSmash(FGameplayEventData Payload);

protected:
	UPROPERTY(EditAnywhere, Category = "Montage")
	TObjectPtr<UAnimMontage> SmashMontage;

protected:
	void ReleaseCatch(AStereoMixPlayerCharacter* TargetCharacter);
};
