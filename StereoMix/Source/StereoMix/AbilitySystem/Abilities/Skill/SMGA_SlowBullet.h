// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGA_Skill.h"
#include "SMGA_SlowBullet.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMGA_SlowBullet : public USMGA_Skill
{
	GENERATED_BODY()

public:
	USMGA_SlowBullet();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void OnMontageEnded();
	
	UFUNCTION(Server, Reliable)
	void ServerRPCLaunchProjectile(const FVector_NetQuantize10& SourceLocation, const FVector_NetQuantize10& TargetLocation);

	UPROPERTY(EditAnywhere, Category = "Design")
	float Damage = 5.0f;
	
	UPROPERTY(EditAnywhere, Category = "Design")
	int32 MaxDistanceByTile = 8;

	UPROPERTY(EditAnywhere, Category = "Design")
	float ProjectileSpeed = 3500.0f;

	UPROPERTY(EditAnywhere, Category = "Design")
	float SlowDebuffMultiplier = 0.3f;

	UPROPERTY(EditAnywhere, Category = "Design")
	float SlowDebuffDuration = 1.5f;
};
