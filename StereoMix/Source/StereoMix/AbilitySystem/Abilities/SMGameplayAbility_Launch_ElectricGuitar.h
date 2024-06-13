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

	void LaunchTimerCallback();

	UPROPERTY(EditAnywhere, Category = "Projectile|ElectricGuitar", DisplayName = "시전 당 투사체 개수")
	int32 ProjectileCount = 3;

	UPROPERTY(EditAnywhere, Category = "Projectile|ElectricGuitar", DisplayName = "투사체가 발사될 넓이")
	float ProjectileWidth = 50.0f;

	UPROPERTY(EditAnywhere, Category = "Projectile|ElectricGuitar", DisplayName = "투사체가 발사될 각도")
	float ProjectileTotalAngle = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Projectile|ElectricGuitar", DisplayName = "전탄 발사 소요 시간")
	float TotalLaunchTime = 0.175f;

public:
	struct FLaunchData
	{
		TArray<FVector> OffsetLocations;
		TArray<FVector> Angles;
		FVector CenterOffsetLocation;
		FVector Nomal;
		int32 Count = 0;
		float Rate = 0.0f;
	};

protected:
	FLaunchData LaunchData;
};
