// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGameplayAbility_Launch_Single.h"
#include "SMGameplayAbility_Launch_Bass.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMGameplayAbility_Launch_Bass : public USMGameplayAbility_Launch_Single
{
	GENERATED_BODY()

protected:
	virtual void LaunchProjectile(const FVector& InStartLocation, const FVector& InNormal) override;
};
