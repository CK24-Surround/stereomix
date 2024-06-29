// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "SMValidationBlueprintLibrary.generated.h"

/**
 *
 */
UCLASS()
class STEREOMIX_API USMValidationBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Validation")
	static bool ValidateName(const FString& Name);

	UFUNCTION(BlueprintCallable, Category = "Validation")
	static bool ValidateRoomCode(const FString& RoomCode);
};
