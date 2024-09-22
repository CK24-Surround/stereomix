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

	void OnChargeBlocked(AActor* TargetActor);

	UFUNCTION()
	void OnChargeEndEntry(FGameplayEventData Payload);

	UFUNCTION()
	void OnChargeEnded();

	UFUNCTION(Server, Reliable)
	void ServerRequestEffect(AActor* TargetActor);

	UPROPERTY(EditAnywhere, Category = "Design")
	FGameplayTagContainer ChargeHitIgnoreTags;

	/** 돌진 시 초당 회전 각입니다.*/
	UPROPERTY(EditAnywhere, Category = "Design")
	float RotationPerSecond = 45.0f;

	/** 스턴 지속시간입니다. */
	float StunTime = 0.0f;

	FName OriginalCapsuleCollisionProfileName;

	FName OriginalChargeCollisionProfileName;
};
