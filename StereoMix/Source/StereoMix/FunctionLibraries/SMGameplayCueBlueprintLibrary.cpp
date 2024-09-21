// Copyright Studio Surround. All Rights Reserved.


#include "SMGameplayCueBlueprintLibrary.h"

#include "AbilitySystem/GameplayCues/SMGameplayCueNotify.h"

USceneComponent* USMGameplayCueBlueprintLibrary::GetTargetComponent(const FGameplayCueParameters& Parameters)
{
	return Parameters.TargetAttachComponent.Get();
}

void USMGameplayCueBlueprintLibrary::GetLocationAndRotation(const FGameplayCueParameters& Parameters, FVector& OutLocation, FRotator& OutRotation)
{
	OutLocation = Parameters.Location;

	OutRotation = FRotator::ZeroRotator;
	if (!Parameters.Normal.IsNearlyZero())
	{
		OutRotation = Parameters.Normal.Rotation();
	}
}
