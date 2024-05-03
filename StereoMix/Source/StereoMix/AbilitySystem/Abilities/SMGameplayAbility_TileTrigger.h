// Copyright Surround, Inc. All Rights Reserved.

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
	virtual void TileTrigger() PURE_VIRTUAL(TileTrigger)

protected:
	FVector TriggerdTileLocation;
	ESMTeam SourceTeam = ESMTeam::None;

	// 타일이 트리거 되야할 규모입니다.
	float TileTriggerMagnitude = 0.0f;
	float TileHorizonSize = 0.0f;
};
