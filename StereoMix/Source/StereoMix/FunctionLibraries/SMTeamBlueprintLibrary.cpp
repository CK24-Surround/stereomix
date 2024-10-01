// Copyright Surround, Inc. All Rights Reserved.


#include "SMTeamBlueprintLibrary.h"

#include "Interfaces/SMTeamInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Utilities/SMLog.h"

ESMTeam USMTeamBlueprintLibrary::GetTeam(const AActor* SourceActor)
{
	const ISMTeamInterface* SourceTeamInterface = Cast<ISMTeamInterface>(SourceActor);
	if (!SourceTeamInterface)
	{
		return ESMTeam::None;
	}

	ESMTeam Team = SourceTeamInterface->GetTeam();
	return Team;
}

bool USMTeamBlueprintLibrary::IsSameTeam(const AActor* lhs, const AActor* rhs)
{
	return GetTeam(lhs) == GetTeam(rhs);
}

ESMLocalTeam USMTeamBlueprintLibrary::GetLocalTeam(AActor* SourceActor)
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

bool USMTeamBlueprintLibrary::IsSameLocalTeam(AActor* SourceActor)
{
	const ESMLocalTeam LocalTeam = GetLocalTeam(SourceActor);
	return LocalTeam == ESMLocalTeam::Equal;
}
