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

ESMLocalTeam USMTeamBlueprintLibrary::GetLocalTeam(AActor* TargetActor)
{
	const ISMTeamInterface* TargetTeamInterface = Cast<ISMTeamInterface>(TargetActor);
	const APlayerController* LocalPlayerController = UGameplayStatics::GetPlayerController(TargetActor, 0);
	const ISMTeamInterface* LocalTeamInterface = LocalPlayerController ? Cast<ISMTeamInterface>(LocalPlayerController->GetPawn()) : nullptr;
	if (!TargetTeamInterface || !LocalTeamInterface)
	{
		return ESMLocalTeam::Different;
	}

	const ESMTeam TargetTeam = TargetTeamInterface->GetTeam();
	const ESMTeam LocalTeam = LocalTeamInterface->GetTeam();
	if (TargetTeam != LocalTeam)
	{
		return ESMLocalTeam::Different;
	}

	return ESMLocalTeam::Equal;
}

bool USMTeamBlueprintLibrary::IsSameLocalTeam(AActor* TargetActor)
{
	const ESMLocalTeam LocalTeam = GetLocalTeam(TargetActor);
	return LocalTeam == ESMLocalTeam::Equal;
}
