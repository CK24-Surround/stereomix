// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGameplayAbility_Launch_Single.h"

#include "SMGameplayAbility_Launch_Piano.generated.h"

struct FPianoLaunchData
{
	FVector SourceLocation;
	TArray<FRotator> SpawnRotationOffset;
	int32 Count = 0;
	float Rate = 0.0f;
	uint32 bIsInverse : 1 = false;
};

/**
 *
 */
UCLASS(Abstract)
class STEREOMIX_API USMGameplayAbility_Launch_Piano : public USMGameplayAbility_Launch_Single
{
	GENERATED_BODY()

protected:
	virtual void LaunchProjectile(const FVector& InStartLocation, const FVector& InNormal) override;

	void LaunchProjectileCallback();

	UPROPERTY(EditAnywhere, Category = "Design|Piano")
	int32 ProjectileCount = 5;

	UPROPERTY(EditAnywhere, Category = "Design|Piano")
	float ProjectileTotalAngle = 45.0f;

	UPROPERTY(EditAnywhere, Category = "Design|Piano")
	float TotalLaunchTime = 0.25f;

	UPROPERTY(EditAnywhere, Category = "Design|Piano")
	uint32 bIsAttach : 1 = true;

	FPianoLaunchData LaunchData;
};
