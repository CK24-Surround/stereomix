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

	UPROPERTY(EditAnywhere, Category = "Design")
	int32 MaxDistanceByTile = 12;

	UPROPERTY(EditAnywhere, Category = "Design")
	float TravelTime = 0.25f;

	FVector TargetLocation;

	UPROPERTY(EditAnywhere, Category = "Design")
	float Damage = 10.0f;
	
	UPROPERTY(EditAnywhere, Category = "Design")
	float Radius = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Design")
	float Magnitude = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "Design")
	uint32 bUseDebug:1 = false;
};
