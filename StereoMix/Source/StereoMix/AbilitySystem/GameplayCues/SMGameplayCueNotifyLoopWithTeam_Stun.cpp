// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayCueNotifyLoopWithTeam_Stun.h"

#include "NiagaraFunctionLibrary.h"

void ASMGameplayCueNotifyLoopWithTeam_Stun::OnStartFX(AActor* SourceActor, const FGameplayCueParameters& Parameters) const
{
	if (!ensureAlways(SourceActor))
	{
		return;
	}

	FVector Location;
	FRotator Rotation;
	GetLocationAndRotation(Parameters, Location, Rotation);

	const ESMTeam Team = GetTeamForSource(Parameters.Instigator.Get());

	USceneComponent* AttachToComponent = SourceActor->GetRootComponent();
	if (!ensureAlways(AttachToComponent))
	{
		return;
	}

	UNiagaraFunctionLibrary::SpawnSystemAttached(StartFX[Team], AttachToComponent, NAME_None, Location, Rotation, EAttachLocation::SnapToTarget, false, true, ENCPoolMethod::AutoRelease);
}

void ASMGameplayCueNotifyLoopWithTeam_Stun::OnLoopingStartFX(AActor* SourceActor, const FGameplayCueParameters& Parameters)
{
	FVector Location;
	FRotator Rotation;
	GetLocationAndRotation(Parameters, Location, Rotation);

	const ESMTeam Team = GetTeamForSource(Parameters.Instigator.Get());

	USceneComponent* AttachToComponent = Parameters.TargetAttachComponent.Get();
	if (!ensureAlways(AttachToComponent))
	{
		return;
	}

	LoopingFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(LoopFX[Team], AttachToComponent, NAME_None, Location, Rotation, EAttachLocation::SnapToTarget, false, true, ENCPoolMethod::AutoRelease);
}

void ASMGameplayCueNotifyLoopWithTeam_Stun::OnEndFX(AActor* SourceActor, const FGameplayCueParameters& Parameters) const
{
	if (!ensureAlways(SourceActor))
	{
		return;
	}

	FVector Location;
	FRotator Rotation;
	GetLocationAndRotation(Parameters, Location, Rotation);

	const ESMTeam Team = GetTeamForSource(Parameters.Instigator.Get());

	USceneComponent* AttachToComponent = SourceActor->GetRootComponent();
	if (!ensureAlways(AttachToComponent))
	{
		return;
	}

	UNiagaraFunctionLibrary::SpawnSystemAttached(EndFX[Team], AttachToComponent, NAME_None, Location, Rotation, EAttachLocation::SnapToTarget, false, true, ENCPoolMethod::AutoRelease);
}
