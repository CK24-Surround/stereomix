// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Factories/BlueprintFunctionLibraryFactory.h"
#include "SMCatchInteractionBlueprintLibrary.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMCatchInteractionBlueprintLibrary : public UBlueprintFunctionLibraryFactory
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "CatchInteraction")
	static USMCatchInteractionComponent* GetCatchInteractionComponent(AActor* InActor);
};
