// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SMGameplayAbility.h"
#include "SMGameplayAbility_Smash.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMGameplayAbility_Smash : public USMGameplayAbility
{
	GENERATED_BODY()

public:
	USMGameplayAbility_Smash();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
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
	void ReleaseCatch(ASMPlayerCharacter* TargetCharacter);
};
