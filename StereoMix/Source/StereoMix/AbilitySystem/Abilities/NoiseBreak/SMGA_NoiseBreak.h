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

	float GetMaxDistance() const { return MaxDistanceByTile * 150.0f; }

protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/** 스플래시 데미지를 적용할때 호출합니다. */
	virtual void ApplySplash(const FVector& TargetLocation);

	UPROPERTY(EditAnywhere, Category = "Design")
	float Damage = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Design")
	int32 CaptureSize = 3;

	UPROPERTY(EditAnywhere, Category = "Design")
	int32 MaxDistanceByTile = 5;
};
