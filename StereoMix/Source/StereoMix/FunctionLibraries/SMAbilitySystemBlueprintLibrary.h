// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "SMAbilitySystemBlueprintLibrary.generated.h"

class USMAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class STEREOMIX_API USMAbilitySystemBlueprintLibrary : public UAbilitySystemBlueprintLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "Actor"))
	static USMAbilitySystemComponent* GetSMAbilitySystemComponent(AActor* Actor);
};
