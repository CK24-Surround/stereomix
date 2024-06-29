// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "SMPlatformBlueprintLibrary.generated.h"

/**
 *
 */
UCLASS()
class STEREOMIX_API USMPlatformBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Platform")
	static void CopyToClipboard(const FString& Text);
};
