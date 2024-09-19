// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGA_NoiseBreak.h"
#include "SMGA_ElectricGuitarNoiseBreak.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMGA_ElectricGuitarNoiseBreak : public USMGA_NoiseBreak
{
	GENERATED_BODY()

public:
	USMGA_ElectricGuitarNoiseBreak();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(Server, Reliable)
	void ServerSendLocationData(const FVector_NetQuantize10& SourceLocation, const FVector_NetQuantize10& TargetLocation);

	void LeapCharacter(const FVector& InStartLocation, const FVector& InTargetLocation, float InGravityZ);

	void OnBurst(const FVector& TargetLocation);

	void OnLanded();

	UFUNCTION()
	void OnNoiseBreakEnded();

	void TileCapture(const FVector& TargetLocation);

	/** 도약하는 동안 적용할 중력 스케일입니다. */
	float NoiseBreakGravityScale = 0.0f;

	/** 도약 정점의 높이입니다. */
	float ApexHeight = 0.0f;

	FName OriginalCollisionProfileName;
};
