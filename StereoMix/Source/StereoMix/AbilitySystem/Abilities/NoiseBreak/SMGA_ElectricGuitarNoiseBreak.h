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

	UFUNCTION()
	void OnReceivedFlashEvent(FGameplayEventData Payload);

	UFUNCTION(Server, Reliable)
	void ServerSendTargetLocation(const FVector_NetQuantize10& StartLocation, const FVector_NetQuantize10& TargetLocation);

	void OnFlash();

	UFUNCTION()
	void OnNoiseBreak();

	void TileCapture();

	UFUNCTION()
	void OnNoiseBreakEnded();

	FName OriginalCollisionProfileName;
	
	FVector NoiseBreakStartLocation;

	FVector NoiseBreakTargetLocation;

	UPROPERTY(EditAnywhere, Category = "Design")
	float FlashDelayTime = 0.3f;

	UPROPERTY(EditAnywhere, Category = "Design")
	int32 DivideCount = 9;
};
