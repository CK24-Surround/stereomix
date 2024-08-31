// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HoldInteraction/SMHoldInteractionComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SMHoldInteractionBlueprintLibrary.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMHoldInteractionBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HIC")
	static USMHoldInteractionComponent* GetHoldInteractionComponent(AActor* InActor);
};
