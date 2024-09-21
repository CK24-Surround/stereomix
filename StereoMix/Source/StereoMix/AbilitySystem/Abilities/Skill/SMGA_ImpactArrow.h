// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGA_Skill.h"
#include "SMGA_ImpactArrow.generated.h"

class USMAT_SkillIndicator;
class UEnhancedInputComponent;

/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMGA_ImpactArrow : public USMGA_Skill
{
	GENERATED_BODY()

public:
	USMGA_ImpactArrow();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	void Shoot();

	UFUNCTION(Server, Reliable)
	void ServerRPCApplyCost();

	UFUNCTION()
	void OnImpact();

	UFUNCTION(Server, Reliable)
	void ServerRPCOnImpact(const FVector_NetQuantize10& NewTargetLocation);

	void SyncPointImpactEnd();

	UPROPERTY()
	TObjectPtr<USMAT_SkillIndicator> SkillIndicatorTask;

	UPROPERTY()
	TObjectPtr<UEnhancedInputComponent> InputComponent;

	/** 충격화살이 떨어지기까지 선 딜레이입니다. */
	float StartUpTime = 0.0f;

	FVector TargetLocation;

	/** 충격화살의 효과 범위입니다. */
	float Radius = 0.0f;

	/** 넉백의 힘입니다. */
	float KnockbackMagnitude = 0.0f;

	uint32 bHasClicked:1 = false;

	UPROPERTY(EditAnywhere, Category = "Design")
	uint32 bUseDebug:1 = false;
};
