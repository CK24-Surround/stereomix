// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SMGameplayAbility.h"
#include "Utilities/SMTeam.h"
#include "SMGameplayAbility_TileTrigger.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMGameplayAbility_TileTrigger : public USMGameplayAbility
{
	GENERATED_BODY()

public:
	USMGameplayAbility_TileTrigger();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	virtual void TileTrigger(const UObject* TriggerFX) PURE_VIRTUAL(TileTrigger)

protected:
	FVector TriggerdTileLocation;
	ESMTeam SourceTeam = ESMTeam::None;

	float TileHorizonSize = 0.0f;
};
