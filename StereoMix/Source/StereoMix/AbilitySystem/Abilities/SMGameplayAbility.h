// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "SMGameplayAbility.generated.h"

class USMAbilitySystemComponent;
class ASMPlayerCharacter;

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	USMGameplayAbility();

public:
	USMAbilitySystemComponent* GetStereoMixAbilitySystemComponentFromActorInfo() const;
	
	ASMPlayerCharacter* GetStereoMixPlayerCharacterFromActorInfo() const;
};
