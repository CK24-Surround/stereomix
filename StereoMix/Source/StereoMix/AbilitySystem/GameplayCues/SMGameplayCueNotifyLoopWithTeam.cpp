// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayCueNotifyLoopWithTeam.h"

#include "Components/SMTeamComponent.h"
#include "Interfaces/SMTeamComponentInterface.h"


ASMGameplayCueNotifyLoopWithTeam::ASMGameplayCueNotifyLoopWithTeam()
{
	bAutoDestroyOnRemove = true;
	bAllowMultipleWhileActiveEvents = false;
	NumPreallocatedInstances = 6;

	StartFX.Add(ESMTeam::None, nullptr);
	StartFX.Add(ESMTeam::EDM, nullptr);
	StartFX.Add(ESMTeam::FutureBass, nullptr);

	LoopFX.Add(ESMTeam::None, nullptr);
	LoopFX.Add(ESMTeam::EDM, nullptr);
	LoopFX.Add(ESMTeam::FutureBass, nullptr);

	EndFX.Add(ESMTeam::None, nullptr);
	EndFX.Add(ESMTeam::EDM, nullptr);
	EndFX.Add(ESMTeam::FutureBass, nullptr);
}

void ASMGameplayCueNotifyLoopWithTeam::GetLocationAndRotation(const FGameplayCueParameters& Parameters, FVector& OutLocation, FRotator& OutRotation) const
{
	OutLocation = Parameters.Location;
	if (Parameters.Normal.IsNearlyZero())
	{
		OutRotation = FRotator::ZeroRotator;
	}
	else
	{
		OutRotation = Parameters.Normal.Rotation();
	}
}

ESMTeam ASMGameplayCueNotifyLoopWithTeam::GetTeamForSource(const AActor* SourceActor) const
{
	const ISMTeamComponentInterface* SourceTeamComponentInterface = Cast<ISMTeamComponentInterface>(SourceActor);
	if (!ensureAlways(SourceTeamComponentInterface))
	{
		return ESMTeam();
	}

	ESMTeam Team = SourceTeamComponentInterface->GetTeamComponent()->GetTeam();
	return Team;
}
