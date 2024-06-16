// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayCueNotifyBase.h"

#include "Interfaces/SMTeamInterface.h"
#include "Kismet/GameplayStatics.h"

void USMGameplayCueNotifyBase::GetLocationAndRotation(const FGameplayCueParameters& Parameters, FVector& OutLocation, FRotator& OutRotation) const
{
	OutLocation = Parameters.Location;

	OutRotation = FRotator::ZeroRotator;
	if (!Parameters.Normal.IsNearlyZero())
	{
		OutRotation = Parameters.Normal.Rotation();
	}
}
