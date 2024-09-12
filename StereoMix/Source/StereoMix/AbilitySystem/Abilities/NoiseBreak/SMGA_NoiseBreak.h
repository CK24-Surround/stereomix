// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SMGameplayAbility.h"
#include "SMGA_NoiseBreak.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMGA_NoiseBreak : public USMGameplayAbility
{
	GENERATED_BODY()

public:
	USMGA_NoiseBreak();

protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;

	virtual void ApplySplash(const FVector& TargetLocation);

	UPROPERTY(EditAnywhere, Category = "Design")
	float Damage = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Design")
	int32 CaptureSize = 3;

	UPROPERTY(EditAnywhere, Category = "Design")
	int32 MaxDistanceByTile = 5;
};
