// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayCueNotify_WithLocalTeam.h"

#include "Data/SMTeam.h"
#include "NiagaraFunctionLibrary.h"
#include "Interfaces/SMTeamInterface.h"
#include "Kismet/GameplayStatics.h"

USMGameplayCueNotify_WithLocalTeam::USMGameplayCueNotify_WithLocalTeam()
{
	FX.FindOrAdd(ESMLocalTeam::Equal, nullptr);
	FX.FindOrAdd(ESMLocalTeam::different, nullptr);
}

void USMGameplayCueNotify_WithLocalTeam::PlayNiagaraSystem(AActor* SourceActor, const FGameplayCueParameters& Parameters) const
{
	FVector Location;
	FRotator Rotation;
	GetLocationAndRotation(Parameters, Location, Rotation);

	ESMLocalTeam Team = ESMLocalTeam::different;
	if (IsSameTeamWithLocalTeam(SourceActor))
	{
		Team = ESMLocalTeam::Equal;
	}

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(SourceActor, FX[Team], Location, Rotation, FVector(1.0), false, true, ENCPoolMethod::AutoRelease);
}

void USMGameplayCueNotify_WithLocalTeam::PlayNiagaraSystemWithAttach(AActor* SourceActor, const FGameplayCueParameters& Parameters) const
{
	FVector Location;
	FRotator Rotation;
	GetLocationAndRotation(Parameters, Location, Rotation);

	ESMLocalTeam Team = ESMLocalTeam::different;
	if (IsSameTeamWithLocalTeam(SourceActor))
	{
		Team = ESMLocalTeam::Equal;
	}

	USceneComponent* AttachToComponent = Parameters.TargetAttachComponent.Get();
	if (!ensureAlways(AttachToComponent))
	{
		return;
	}

	UNiagaraFunctionLibrary::SpawnSystemAttached(FX[Team], AttachToComponent, NAME_None, Location, Rotation, EAttachLocation::SnapToTarget, false, true, ENCPoolMethod::AutoRelease);
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
