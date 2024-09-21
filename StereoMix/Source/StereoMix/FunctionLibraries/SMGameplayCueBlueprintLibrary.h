// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SMGameplayCueBlueprintLibrary.generated.h"

struct FGameplayCueParameters;

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMGameplayCueBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** 큐 파라미터에서 타겟 컴포넌트를 가져옵니다. */
	UFUNCTION(BlueprintCallable, Category = "GameplayCue")
	static USceneComponent* GetTargetComponent(const FGameplayCueParameters& Parameters);

	/** 큐 파라미터에서 위치와 회전값을 가져옵니다. */
	UFUNCTION(BlueprintCallable, Category = "GameplayCue")
	static void GetLocationAndRotation(const FGameplayCueParameters& Parameters, FVector& OutLocation, FRotator& OutRotation);
};
