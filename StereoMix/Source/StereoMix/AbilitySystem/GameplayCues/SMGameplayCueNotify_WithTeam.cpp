// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayCueNotify_WithTeam.h"

#include "NiagaraFunctionLibrary.h"
#include "Components/SMTeamComponent.h"
#include "Interfaces/SMTeamComponentInterface.h"
#include "Utilities/SMLog.h"

USMGameplayCueNotify_WithTeam::USMGameplayCueNotify_WithTeam()
{
	FX.Add(ESMTeam::None, nullptr);
	FX.Add(ESMTeam::EDM, nullptr);
	FX.Add(ESMTeam::FutureBass, nullptr);
}

void USMGameplayCueNotify_WithTeam::PlayNiagaraSystem(AActor* SourceActor, const FGameplayCueParameters& Parameters) const
{
	const FVector Location = Parameters.Location;
	FRotator Rotation;
	if (Parameters.Normal.IsNearlyZero())
	{
		Rotation = FRotator::ZeroRotator;
	}
	else
	{
		Rotation = Parameters.Normal.Rotation();
	}

	ISMTeamComponentInterface* SourceTeamComponentInterface = Cast<ISMTeamComponentInterface>(SourceActor);
	if (!ensureAlways(SourceTeamComponentInterface))
	{
		return;
	}

	ESMTeam Team = SourceTeamComponentInterface->GetTeamComponent()->GetTeam();
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(SourceActor, FX[Team], Location, Rotation, FVector(1.0), false, true, ENCPoolMethod::AutoRelease);
}
