// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayCueNotify_WithLocalTeam.h"

#include "Data/SMTeam.h"
#include "FMODBlueprintStatics.h"
#include "Interfaces/SMTeamInterface.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

USMGameplayCueNotify_WithLocalTeam::USMGameplayCueNotify_WithLocalTeam()
{
	Effect.FindOrAdd(ESMLocalTeam::Equal, nullptr);
	Effect.FindOrAdd(ESMLocalTeam::Different, nullptr);
}

void USMGameplayCueNotify_WithLocalTeam::PlayNiagaraSystem(AActor* SourceActor, const FGameplayCueParameters& Parameters) const
{
	ESMLocalTeam Team = ESMLocalTeam::Different;
	if (IsSameTeamWithLocalTeam(SourceActor))
	{
		Team = ESMLocalTeam::Equal;
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

void USMGameplayCueNotify_WithLocalTeam::PlayNiagaraSystemWithAttach(AActor* SourceActor, const FGameplayCueParameters& Parameters) const
{
	ESMLocalTeam Team = ESMLocalTeam::Different;
	if (IsSameTeamWithLocalTeam(SourceActor))
	{
		Team = ESMLocalTeam::Equal;
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

void USMGameplayCueNotify_WithLocalTeam::PlaySound(AActor* SourceActor, const FGameplayCueParameters& Parameters) const
{
	ESMLocalTeam Team = ESMLocalTeam::Different;
	if (IsSameTeamWithLocalTeam(SourceActor))
	{
		Team = ESMLocalTeam::Equal;
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

void USMGameplayCueNotify_WithLocalTeam::PlaySoundWithAttach(AActor* SourceActor, const FGameplayCueParameters& Parameters) const
{
	ESMLocalTeam Team = ESMLocalTeam::Different;
	if (IsSameTeamWithLocalTeam(SourceActor))
	{
		Team = ESMLocalTeam::Equal;
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

bool USMGameplayCueNotify_WithLocalTeam::IsSameTeamWithLocalTeam(AActor* SourceActor) const
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
