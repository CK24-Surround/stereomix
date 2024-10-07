// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SMPlayerControllerDataAsset.generated.h"

class USMUserWidget_ScreenIndicator;

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMPlayerControllerDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<USMUserWidget_ScreenIndicator> ScreenIndicatorClass;
};
