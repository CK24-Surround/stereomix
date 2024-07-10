// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "SMControlData.generated.h"

class UInputAction;
class UInputMappingContext;
/**
 *
 */
UCLASS()
class STEREOMIX_API USMControlData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<const UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<const UInputMappingContext> UIMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<const UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<const UInputAction> ShootAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<const UInputAction> CatchAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<const UInputAction> SmashAction;
};
