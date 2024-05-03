// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayCueNotify_Actor_WithTeam.h"

#include "NiagaraFunctionLibrary.h"
#include "Components/SMTeamComponent.h"
#include "Interfaces/SMTeamComponentInterface.h"

ASMGameplayCueNotify_Actor_WithTeam::ASMGameplayCueNotify_Actor_WithTeam()
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

void ASMGameplayCueNotify_Actor_WithTeam::PlayOnActivateNiagaraSystemWithAttach(AActor* SourceActor, const FGameplayCueParameters& Parameters) const
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

	USceneComponent* AttachToComponent = SourceActor->GetRootComponent();
	if (!ensureAlways(AttachToComponent))
	{
		return;
	}

	UNiagaraFunctionLibrary::SpawnSystemAttached(StartFX[Team], AttachToComponent, NAME_None, Location, Rotation, EAttachLocation::SnapToTarget, false, true, ENCPoolMethod::AutoRelease);
}

void ASMGameplayCueNotify_Actor_WithTeam::PlayOnLoopingStartNiagaraSystemWithAttach(AActor* SourceActor, const FGameplayCueParameters& Parameters)
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

	LoopingFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(LoopFX[Team], AttachToComponent, NAME_None, Location, Rotation, EAttachLocation::SnapToTarget, false, true, ENCPoolMethod::AutoRelease);
}

void ASMGameplayCueNotify_Actor_WithTeam::PlayOnRemoveNiagaraSystemWithAttach(AActor* SourceActor, const FGameplayCueParameters& Parameters) const
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

	USceneComponent* AttachToComponent = SourceActor->GetRootComponent();
	if (!ensureAlways(AttachToComponent))
	{
		return;
	}

	UNiagaraFunctionLibrary::SpawnSystemAttached(EndFX[Team], AttachToComponent, NAME_None, Location, Rotation, EAttachLocation::SnapToTarget, false, true, ENCPoolMethod::AutoRelease);
}

void ASMGameplayCueNotify_Actor_WithTeam::GetLocationAndRotation(const FGameplayCueParameters& Parameters, FVector& OutLocation, FRotator& OutRotation) const
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

ESMTeam ASMGameplayCueNotify_Actor_WithTeam::GetTeamForSource(const AActor* SourceActor) const
{
	const ISMTeamComponentInterface* SourceTeamComponentInterface = Cast<ISMTeamComponentInterface>(SourceActor);
	if (!ensureAlways(SourceTeamComponentInterface))
	{
		return ESMTeam();
	}

	ESMTeam Team = SourceTeamComponentInterface->GetTeamComponent()->GetTeam();
	return Team;
}
