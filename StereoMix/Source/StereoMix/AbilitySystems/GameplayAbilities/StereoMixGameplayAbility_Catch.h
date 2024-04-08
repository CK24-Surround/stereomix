// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Characters/StereoMixPlayerCharacter.h"
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

	UPROPERTY(EditAnywhere, Category = "Tag")
	FGameplayTagContainer CatchableTags;

protected:
	UFUNCTION()
	void OnCancelled();

	UFUNCTION()
	void OnInterrupted();

	UFUNCTION()
	void OnBlendOut();

protected:
	UFUNCTION()
	void OnEventReceived(FGameplayEventData Payload);

protected:
	UFUNCTION(Server, Reliable)
	void ServerRPCRequestTargetOverlap(const FVector_NetQuantize10& InStartLocation, const FVector_NetQuantize10& InCursorLocation);

	TArray<AStereoMixPlayerCharacter*> GetCatchableCharacters(const TArray<FOverlapResult>& InOverlapResults);

protected:
	FVector StartLocation;
	FVector TargetLocation;
};
