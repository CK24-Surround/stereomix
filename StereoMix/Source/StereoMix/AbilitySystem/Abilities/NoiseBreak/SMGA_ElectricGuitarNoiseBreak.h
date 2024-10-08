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
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void OnFlashEventReceived(FGameplayEventData Payload);

	UFUNCTION(Server, Reliable)
	void ServerSendTargetLocation(const FVector_NetQuantize10& StartLocation, const FVector_NetQuantize10& TargetLocation);

	void OnFlash();

	UFUNCTION()
	void OnNoiseBreakBurst();

	TArray<ASMTile*> GetTilesToBeCaptured(const UWorld* World) const;

	void TileCapture();

	/** 일렉기타용으로 커스텀한 버스트 공격 수행입니다. */
	void PerformElectricGuitarBurstAttack();

	/** 일렉기타용으로 커스텀한 버스트 적중 액터 반환 함수입니다. */
	TArray<AActor*> GetElectricGuitarBurstHitActors();

	UFUNCTION()
	void SyncPointNoiseBreakEnded();

	FName OriginalCollisionProfileName;

	FVector NoiseBreakStartLocation;

	FVector NoiseBreakTargetLocation;

	UPROPERTY(EditAnywhere, Category = "Design")
	float BurstDelayAfterFlash = 0.3f;

	UPROPERTY(EditAnywhere, Category = "Design")
	float EndDelayAfterFlash = 0.5f;
};
