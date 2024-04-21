// Fill out your copyright notice in the Description page of Project Settings.


#include "SMCalculateBlueprintLibrary.h"

FVector USMCalculateBlueprintLibrary::SuggestProjectileVelocity_CustomApexHeight(const UObject* WorldContextObject, const FVector& StartLocation, const FVector& EndLocation, float ApexHeight, float OverrideGravityZ)
{
	if (!ensureAlways(GEngine))
	{
		return FVector::ZeroVector;
	}

	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!ensureAlways(World))
	{
		return FVector::ZeroVector;
	}

	const FVector StartToEnd = EndLocation - StartLocation;
	const float StartToEndDistance = StartToEnd.Size();

	const float GravityZ = FMath::IsNearlyEqual(OverrideGravityZ, 0.0f) ? World->GetGravityZ() : OverrideGravityZ;

	const float TimeToApex = FMath::Sqrt(2 * ApexHeight / -GravityZ);
	const float VerticalVelocity = -GravityZ * TimeToApex; 
	
	if (StartToEndDistance <= UE_KINDA_SMALL_NUMBER)
	{
		return FVector(0.0, 0.0, VerticalVelocity);
	}

	const float HorizontalVelocity = StartToEnd.Size2D() / (2 * TimeToApex);

	FVector HorizontalDirection = StartToEnd.GetSafeNormal2D();
	const FVector LaunchVelocity = FVector(HorizontalDirection.X * HorizontalVelocity, HorizontalDirection.Y * HorizontalVelocity, VerticalVelocity);

	return LaunchVelocity;
}
