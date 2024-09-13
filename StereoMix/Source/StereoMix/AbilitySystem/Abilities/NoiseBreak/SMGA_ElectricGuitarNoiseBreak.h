// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGA_NoiseBreak.h"
#include "SMGA_ElectricGuitarNoiseBreak.generated.h"

/**
 * 
 */
UCLASS()
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

	UPROPERTY(EditAnywhere, Category = "Design")
	float ApexHegith = 300.0f;

	UPROPERTY(EditAnywhere, Category = "Design")
	float NoiseBreakGravityScale = 5.0f;

	FName OriginalCollisionProfileName;
};
