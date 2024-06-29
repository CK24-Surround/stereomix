// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayCueNotify_WithTeam.h"

#include "FMODBlueprintStatics.h"
#include "Interfaces/SMTeamInterface.h"
#include "NiagaraFunctionLibrary.h"
#include "Utilities/SMLog.h"

void USMGameplayCueNotify_WithTeam::PlayNiagaraSystem(AActor* SourceActor, const FGameplayCueParameters& Parameters) const
{
	ESMTeam Team;
	if (!bUseInstigatorTeam)
	{
		Team = GetTeamForSource(SourceActor);
	}
	else
	{
		Team = GetTeamForSource(Parameters.Instigator.Get());
	}

	if (!Effect.Find(Team))
	{
		return;
	}

	FVector Location;
	FRotator Rotation;
	GetLocationAndRotation(Parameters, Location, Rotation);

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(SourceActor, Effect[Team], Location, Rotation, FVector(1.0), false, true, ENCPoolMethod::AutoRelease);
}

void USMGameplayCueNotify_WithTeam::PlayNiagaraSystemWithAttach(AActor* SourceActor, const FGameplayCueParameters& Parameters) const
{
	ESMTeam Team;
	if (!bUseInstigatorTeam)
	{
		Team = GetTeamForSource(SourceActor);
	}
	else
	{
		Team = GetTeamForSource(Parameters.Instigator.Get());
	}

	if (!Effect.Find(Team))
	{
		return;
	}

	FVector Location;
	FRotator Rotation;
	GetLocationAndRotation(Parameters, Location, Rotation);

	USceneComponent* AttachToComponent = Parameters.TargetAttachComponent.Get();
	if (!ensureAlways(AttachToComponent))
	{
		return;
	}

	UNiagaraFunctionLibrary::SpawnSystemAttached(Effect[Team], AttachToComponent, NAME_None, Location, Rotation, EAttachLocation::SnapToTarget, false, true, ENCPoolMethod::AutoRelease);
}

void USMGameplayCueNotify_WithTeam::PlaySound(AActor* SourceActor, const FGameplayCueParameters& Parameters) const
{
	ESMTeam Team;
	if (!bUseInstigatorTeam)
	{
		Team = GetTeamForSource(SourceActor);
	}
	else
	{
		Team = GetTeamForSource(Parameters.Instigator.Get());
	}

	if (!Sound.Find(Team))
	{
		return;
	}

	FVector Location;
	FRotator Rotation;
	GetLocationAndRotation(Parameters, Location, Rotation);
	FTransform NewTransform{ Rotation, Location };
	UFMODBlueprintStatics::PlayEventAtLocation(SourceActor, Sound[Team], NewTransform, true);
}

void USMGameplayCueNotify_WithTeam::PlaySoundWithAttach(AActor* SourceActor, const FGameplayCueParameters& Parameters) const
{
	ESMTeam Team;
	if (!bUseInstigatorTeam)
	{
		Team = GetTeamForSource(SourceActor);
	}
	else
	{
		Team = GetTeamForSource(Parameters.Instigator.Get());
	}

	if (!Sound.Find(Team))
	{
		return;
	}

	FVector Location;
	FRotator Rotation;
	GetLocationAndRotation(Parameters, Location, Rotation);

	USceneComponent* AttachToComponent = Parameters.TargetAttachComponent.Get();
	if (!ensureAlways(AttachToComponent))
	{
		return;
	}

	UFMODBlueprintStatics::PlayEventAttached(Sound[Team], AttachToComponent, NAME_None, Location, EAttachLocation::SnapToTarget, true, true, true);
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
