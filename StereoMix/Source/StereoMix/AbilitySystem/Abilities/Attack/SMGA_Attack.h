// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SMGameplayAbility.h"
#include "SMGA_Attack.generated.h"

struct FSMCharacterAttackData;

/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMGA_Attack : public USMGameplayAbility
{
	GENERATED_BODY()

public:
	USMGA_Attack();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	float Damage = 0.0f;

	float MaxDistanceByTile = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Design")
	uint32 bUseOneHitKill:1 = false;
};
