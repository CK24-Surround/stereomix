// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "StereoMixGameplayAbilityTargetActor_Trace.generated.h"

UCLASS()
class STEREOMIX_API AStereoMixGameplayAbilityTargetActor_Trace : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	virtual void StartTargeting(UGameplayAbility* Ability) override;

	virtual void ConfirmTargetingAndContinue() override;
};
