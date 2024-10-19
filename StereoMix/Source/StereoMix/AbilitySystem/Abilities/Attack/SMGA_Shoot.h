// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGA_Attack.h"
#include "SMGA_Shoot.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMGA_Shoot : public USMGA_Attack
{
	GENERATED_BODY()

public:
	USMGA_Shoot();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	void Shoot();

	UFUNCTION(Server, Reliable)
	void ServerRPCApplyCost();

	UFUNCTION(Server, Reliable)
	void ServerRPCLaunchProjectile(const FVector_NetQuantize10& SourceLocation, const FVector_NetQuantize10& TargetLocation);

	float AttackPerSecond = 0.0f;

	float ProjectileSpeed = 0.0f;

	float SpreadAngle = 0.0f;

	int32 AccuracyShootRate = 0;

	FTimerHandle ShootTimerHandle;

	uint32 bIsInputReleased = false;
};
