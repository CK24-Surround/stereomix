// Copyright Surround, Inc. All Rights Reserved.


#include "SMTeamBlueprintLibrary.h"

#include "Interfaces/SMTeamInterface.h"
#include "Kismet/GameplayStatics.h"

bool USMTeamBlueprintLibrary::IsSameTeam(AActor* SourceActor)
{
	ISMTeamInterface* SourceTeamInterface = Cast<ISMTeamInterface>(SourceActor);
	if (!ensureAlways(SourceTeamInterface))
	{
		return false;
	}

	APlayerController* LocalPlayerController = UGameplayStatics::GetPlayerController(SourceActor, 0);
	if (!ensureAlways(LocalPlayerController))
	{
		return false;
	}

	ISMTeamInterface* LocalTeamInterface = Cast<ISMTeamInterface>(LocalPlayerController->GetPawn());
	if (!ensureAlways(LocalTeamInterface))
	{
		return false;
	}

	ESMTeam SourceTeam = SourceTeamInterface->GetTeam();
	ESMTeam LocalTeam = LocalTeamInterface->GetTeam();
	if (SourceTeam != LocalTeam)
	{
		return false;
	}

	return true;
}
