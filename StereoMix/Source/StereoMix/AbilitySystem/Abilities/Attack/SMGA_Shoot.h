// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGA_Attack.h"
#include "SMGA_Shoot.generated.h"


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

	void ShootTick(float DeltaTime);
	void Shoot();

	void LaunchProjectile(const FVector& InLocation, const FRotator& InRotation);

	UFUNCTION(Client, Unreliable)
	void ClientOnShoot();

	void ResetMontage();

	float AttackPerSecond = 0.0f;
	float ProjectileSpeed = 0.0f;
	float SpreadAngle = 0.0f;
	int32 AccuracyShootRate = 0;
	double AttackDelay = 0.0;

	double LastShootTime = 0.0;
};
