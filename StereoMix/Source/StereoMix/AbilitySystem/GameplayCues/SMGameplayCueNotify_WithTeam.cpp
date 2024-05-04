// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayCueNotify_WithTeam.h"

#include "NiagaraFunctionLibrary.h"
#include "Interfaces/SMTeamInterface.h"
#include "Utilities/SMLog.h"

USMGameplayCueNotify_WithTeam::USMGameplayCueNotify_WithTeam()
{
	FX.Add(ESMTeam::None, nullptr);
	FX.Add(ESMTeam::EDM, nullptr);
	FX.Add(ESMTeam::FutureBass, nullptr);
}

void USMGameplayCueNotify_WithTeam::PlayNiagaraSystem(AActor* SourceActor, const FGameplayCueParameters& Parameters) const
{
	FVector Location;
	FRotator Rotation;
	GetLocationAndRotation(Parameters, Location, Rotation);

	ESMTeam Team;
	if (!bUseInstigatorTeam)
	{
		Team = GetTeamForSource(SourceActor);
	}
	else
	{
		Team = GetTeamForSource(Parameters.Instigator.Get());
	}

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(SourceActor, FX[Team], Location, Rotation, FVector(1.0), false, true, ENCPoolMethod::AutoRelease);
}

void USMGameplayCueNotify_WithTeam::PlayNiagaraSystemWithAttach(AActor* SourceActor, const FGameplayCueParameters& Parameters) const
{
	FVector Location;
	FRotator Rotation;
	GetLocationAndRotation(Parameters, Location, Rotation);

	ESMTeam Team;
	if (!bUseInstigatorTeam)
	{
		Team = GetTeamForSource(SourceActor);
	}
	else
	{
		Team = GetTeamForSource(Parameters.Instigator.Get());
	}

	USceneComponent* AttachToComponent = Parameters.TargetAttachComponent.Get();
	if (!ensureAlways(AttachToComponent))
	{
		return;
	}

	UNiagaraFunctionLibrary::SpawnSystemAttached(FX[Team], AttachToComponent, NAME_None, Location, Rotation, EAttachLocation::SnapToTarget, false, true, ENCPoolMethod::AutoRelease);
}

void USMGameplayCueNotify_WithTeam::GetLocationAndRotation(const FGameplayCueParameters& Parameters, FVector& OutLocation, FRotator& OutRotation) const
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

ESMTeam USMGameplayCueNotify_WithTeam::GetTeamForSource(const AActor* SourceActor) const
{
	const ISMTeamInterface* SourceTeamInterface = Cast<ISMTeamInterface>(SourceActor);
	if (!ensureAlways(SourceTeamInterface))
	{
		return ESMTeam();
	}

	ESMTeam Team = SourceTeamInterface->GetTeam();
	return Team;
}
