// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayCueNotifyBase.h"


USMGameplayCueNotifyBase::USMGameplayCueNotifyBase()
{
	for (int32 i = 1; i < static_cast<int32>(ESMTeam::Max); ++i)
	{
		ESMTeam Key = static_cast<ESMTeam>(i);
		VFX.Add(Key, nullptr);
		SFX.Add(Key, nullptr);
	}
}

void USMGameplayCueNotifyBase::GetLocationAndRotation(const FGameplayCueParameters& Parameters, FVector& OutLocation, FRotator& OutRotation) const
{
	OutLocation = Parameters.Location;

	OutRotation = FRotator::ZeroRotator;
	if (!Parameters.Normal.IsNearlyZero())
	{
		OutRotation = Parameters.Normal.Rotation();
	}
}
