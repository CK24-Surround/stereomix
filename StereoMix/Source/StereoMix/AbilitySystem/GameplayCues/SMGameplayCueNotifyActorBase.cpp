// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayCueNotifyActorBase.h"

ASMGameplayCueNotifyActorBase::ASMGameplayCueNotifyActorBase()
{
	PrimaryActorTick.bCanEverTick = false;

	bAutoDestroyOnRemove = true;

	bUniqueInstancePerSourceObject = true;
	bAllowMultipleOnActiveEvents = false;

	for (int32 i = 1; i < static_cast<int32>(ESMTeam::Max); ++i)
	{
		ESMTeam Key = static_cast<ESMTeam>(i);
		VFX.Add(Key, nullptr);
		SFX.Add(Key, nullptr);
	}
}

void ASMGameplayCueNotifyActorBase::GetLocationAndRotation(const FGameplayCueParameters& Parameters, FVector& OutLocation, FRotator& OutRotation) const
{
	OutLocation = Parameters.Location;

	OutRotation = FRotator::ZeroRotator;
	if (!Parameters.Normal.IsNearlyZero())
	{
		OutRotation = Parameters.Normal.Rotation();
	}
}
