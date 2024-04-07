// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "StereoMixGameplayAbility_Catch.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API UStereoMixGameplayAbility_Catch : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UStereoMixGameplayAbility_Catch();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Montage")
	TObjectPtr<UAnimMontage> CatchMontage;

protected:
	UFUNCTION()
	void OnFinished();
	
	UFUNCTION()
	void OnCancelled();
	
	UFUNCTION()
	void OnInterrupted();

protected:
	UFUNCTION()
	void OnEventReceived(FGameplayEventData Payload);

protected:
	UFUNCTION(Server, Reliable)
	void ServerRPCRequestSpawnTargetActor(const FVector_NetQuantize10& StartLocation, const FVector_NetQuantize10& CursorLocation);

	UFUNCTION()
	void OnCompleteTrace(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
};
