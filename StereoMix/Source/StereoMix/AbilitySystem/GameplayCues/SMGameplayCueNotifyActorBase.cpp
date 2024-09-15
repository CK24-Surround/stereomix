// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayCueNotifyActorBase.h"

ASMGameplayCueNotifyActorBase::ASMGameplayCueNotifyActorBase()
{
	PrimaryActorTick.bCanEverTick = false;

	bAutoDestroyOnRemove = true;
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
