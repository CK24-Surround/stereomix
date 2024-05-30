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

ESMTeam USMGameplayCueNotifyBase::GetTeam(const AActor* SourceActor) const
{
	const ISMTeamInterface* SourceTeamInterface = Cast<ISMTeamInterface>(SourceActor);
	if (!ensureAlways(SourceTeamInterface))
	{
		return ESMTeam();
	}

	ESMTeam Team = SourceTeamInterface->GetTeam();
	return Team;
}

ESMLocalTeam USMGameplayCueNotifyBase::GetLocalTeam(AActor* SourceActor) const
{
	ISMTeamInterface* SourceTeamInterface = Cast<ISMTeamInterface>(SourceActor);
	if (!ensureAlways(SourceTeamInterface))
	{
		return ESMLocalTeam::Different;
	}

	APlayerController* LocalPlayerController = UGameplayStatics::GetPlayerController(SourceActor, 0);
	if (!ensureAlways(LocalPlayerController))
	{
		return ESMLocalTeam::Different;
	}

	ISMTeamInterface* LocalTeamInterface = Cast<ISMTeamInterface>(LocalPlayerController->GetPawn());
	if (!ensureAlways(LocalTeamInterface))
	{
		return ESMLocalTeam::Different;
	}

	ESMTeam SourceTeam = SourceTeamInterface->GetTeam();
	ESMTeam LocalTeam = LocalTeamInterface->GetTeam();
	if (SourceTeam != LocalTeam)
	{
		return ESMLocalTeam::Different;
	}

	return ESMLocalTeam::Equal;
}
