// Copyright Studio Surround. All Rights Reserved.


#include "SMAbilitySystemBlueprintLibrary.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"

USMAbilitySystemComponent* USMAbilitySystemBlueprintLibrary::GetSMAbilitySystemComponent(AActor* Actor)
{
	return Cast<USMAbilitySystemComponent>(GetAbilitySystemComponent(Actor));
}
