// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "StereoMixGameplayAbility.generated.h"

class UStereoMixAbilitySystemComponent;
class ASMPlayerCharacter;

/**
 * 
 */
UCLASS()
class STEREOMIX_API UStereoMixGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UStereoMixGameplayAbility();

public:
	UStereoMixAbilitySystemComponent* GetStereoMixAbilitySystemComponentFromActorInfo() const;
	
	ASMPlayerCharacter* GetStereoMixPlayerCharacterFromActorInfo() const;
};
