// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "SMCalculateBlueprintLibrary.generated.h"

/**
 *
 */
UCLASS()
class STEREOMIX_API USMCalculateBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Physics")
	static FVector SuggestProjectileVelocity_CustomApexHeight(const UObject* WorldContextObject, const FVector& StartLocation, const FVector& EndLocation, float ApexHeight, float OverrideGravityZ = 0.0f);

	UFUNCTION(BlueprintCallable, Category = "Physics")
	static FVector SuggestProjectileVelocity_CustomApexHeight2(const UObject* WorldContextObject, const FVector& StartLocation, const FVector& EndLocation, float ApexHeight, float OverrideGravityZ = 0.0f);
};
