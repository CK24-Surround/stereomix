// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGA_NoiseBreak.h"
#include "SMGA_PianoNoiseBreak.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMGA_PianoNoiseBreak : public USMGA_NoiseBreak
{
	GENERATED_BODY()

public:
	USMGA_PianoNoiseBreak();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(Server, Reliable)
	void ServerSendLocationData(const FVector_NetQuantize10& TargetLocation);

	UFUNCTION()
	void OnShoot(FGameplayEventData Payload);

	UFUNCTION()
	void OnNoiseBreakEnded();

	TArray<ASMTile*> GetTilesToBeCaptured();

	FVector NoiseBreakTargetLocation;

	FName OriginalCollisionProfileName;
};
