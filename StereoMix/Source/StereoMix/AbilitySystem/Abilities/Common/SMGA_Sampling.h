// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SMGameplayAbility.h"
#include "Tiles/SMTile.h"
#include "SMGA_Sampling.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMGA_Sampling : public USMGameplayAbility
{
	GENERATED_BODY()

public:
	USMGA_Sampling();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	void OnUnderTileChanged(ASMTile* UnderTile);

	UPROPERTY(EditAnywhere, Category = "Design")
	FGameplayTagContainer DeactivateTags;

	UPROPERTY(EditAnywhere, Category = "Design")
	float TickRate = 10.0f;
};
