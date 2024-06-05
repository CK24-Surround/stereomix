// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UmgAnimationDuration.generated.h"

USTRUCT(BlueprintType)
struct FUmgAnimationDuration
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Duration")
	float StartTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Duration")
	float EndTime = 0.0f;
};
