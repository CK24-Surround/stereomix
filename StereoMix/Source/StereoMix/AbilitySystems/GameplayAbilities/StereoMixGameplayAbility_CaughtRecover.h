// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "StereoMixGameplayAbility_CaughtRecover.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API UStereoMixGameplayAbility_CaughtRecover : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UStereoMixGameplayAbility_CaughtRecover();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditAnywhere, Category = "GAS|GE")
	TSubclassOf<UGameplayEffect> AddUncatchableStateGE;
	
	UPROPERTY(EditAnywhere, Category = "GAS|GE")
	TSubclassOf<UGameplayEffect> RemoveCatchStateGE;

	UPROPERTY(EditAnywhere, Category = "GAS|GE")
	TSubclassOf<UGameplayEffect> RemoveCaughtStateGE;
};
