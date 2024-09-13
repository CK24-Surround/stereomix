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

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	UFUNCTION(Server, Reliable)
	void ServerRPCLaunchProjectile(const FVector_NetQuantize10& SourceLocation, const FVector_NetQuantize10& TargetLocation);

	void Shoot();

	UFUNCTION(Server, Reliable)
	void ServerRPCApplyCost();

	UPROPERTY(EditAnywhere, Category = "Design")
	int32 MaxDistanceByTile = 5;

	UPROPERTY(EditAnywhere, Category = "Design")
	float ShootRate = 6.0f;

	UPROPERTY(EditAnywhere, Category = "Design")
	float ProjectileSpeed = 3000.0f;

	UPROPERTY(EditAnywhere, Category = "Design")
	int32 ShootAngle = 35;

	UPROPERTY(EditAnywhere, Category = "Design")
	int32 AccuracyShootRate = 75;

	FTimerHandle ShootTimerHandle;
};
