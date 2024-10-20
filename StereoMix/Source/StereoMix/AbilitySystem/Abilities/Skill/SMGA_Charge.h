// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGA_Skill.h"
#include "SMGA_Charge.generated.h"

class UAbilityTask_ApplyRootMotionConstantForce;

/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMGA_Charge : public USMGA_Skill
{
	GENERATED_BODY()

public:
	USMGA_Charge();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	void OnChargeBlocked(AActor* TargetActor);

	UFUNCTION(Server, Reliable)
	void ServerRequestEffect(AActor* TargetActor);

	UFUNCTION()
	void OnChargeFailed(FGameplayEventData Payload);

	UFUNCTION()
	void OnChargeEndedSyncPoint();

	/** 돌진 시 초당 회전 각입니다.*/
	UPROPERTY(EditAnywhere, Category = "Design")
	float RotationPerSecond = 90.0f;

	UPROPERTY(EditAnywhere, Category = "Design")
	float ChargeMoveSpeedMultiplier = 1.25f;

	/** 스턴 지속시간입니다. */
	float StunTime = 0.0f;

	FName OriginalCapsuleCollisionProfileName;

	FName OriginalChargeCollisionProfileName;
};
