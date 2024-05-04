// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGameplayAbility_Launch.h"
#include "SMGameplayAbility_Launch_ElectricGuitar.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMGameplayAbility_Launch_ElectricGuitar : public USMGameplayAbility_Launch
{
	GENERATED_BODY()

protected:
	virtual void LaunchProjectile(const FVector& InStartLocation, const FVector& InNormal) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Projectile|ElectricGuitar", DisplayName = "시전 당 투사체 개수")
	int32 ProjectileCount = 3;
};
