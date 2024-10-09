// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGA_NoiseBreak.h"
#include "SMGA_BassNoiseBreak.generated.h"

class UAnimMontage;
class UGameplayEffect;

/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMGA_BassNoiseBreak : public USMGA_NoiseBreak
{
	GENERATED_BODY()

public:
	USMGA_BassNoiseBreak();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(Server, Reliable)
	void ServerSendLocationData(const FVector_NetQuantize10& NewSourceLocation, const FVector_NetQuantize10& NewTargetLocation);

	/** 정점 높이를 기준으로 타겟을 향하는 벨로시티를 통해 캐릭터를 도약 시킵니다. */
	void LeapCharacter(const FVector& InStartLocation, const FVector& InTargetLocation, float InGravityZ);

	/** 매치기의 핵심 로직입니다. 착지 시 호출됩니다. */
	UFUNCTION()
	void OnLanded();

	/** 싱크를 동기화하고 노이즈 브레이크를 종료합니다. */
	UFUNCTION()
	void OnNoiseBreakEnded(FGameplayEventData Payload);

	TArray<ASMTile*> GetTilesToBeCaptured();

	UFUNCTION()
	void OnWeaponTrailActivate(FGameplayEventData Payload);

	UFUNCTION()
	void OnWeaponTrailDeactivate(FGameplayEventData Payload);

	UFUNCTION()
	void OnSlash(FGameplayEventData Payload);

	UPROPERTY()
	TObjectPtr<UAnimMontage> CachedNoiseBreakMontage;

	FVector NoiseBreakStartLocation;
	
	FVector NoiseBreakTargetLocation;

	/** 도약하는 동안 적용할 중력 스케일입니다. */
	float NoiseBreakGravityScale = 3.0f;

	/** 도약 정점의 높이입니다. */
	float ApexHeight = 500.0f;

	FName OriginalCollisionProfileName;
};
