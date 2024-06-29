// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "SMCatchInteractionBlueprintLibrary.generated.h"

class USMCatchInteractionComponent;

/**
 *
 */
UCLASS()
class STEREOMIX_API USMCatchInteractionBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "CatchInteraction")
	static USMCatchInteractionComponent* GetCatchInteractionComponent(AActor* InActor);
};
