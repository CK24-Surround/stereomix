// Copyright Studio Surround. All Rights Reserved.


#include "SMGCN_OneShot.h"

#include "FMODBlueprintStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "FunctionLibraries/SMGameplayCueBlueprintLibrary.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"

bool USMGCN_OneShot::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	if (!MyTarget)
	{
		return false;
	}

	FVector TargetLocation;
	FRotator TargetRotation;
	USMGameplayCueBlueprintLibrary::GetLocationAndRotation(Parameters, TargetLocation, TargetRotation);
	USceneComponent* TargetComponent = Parameters.TargetAttachComponent.Get();

	ESMTeam SourceTeam = USMTeamBlueprintLibrary::GetTeam(MyTarget);
	SourceTeam = SourceTeam != ESMTeam::None ? SourceTeam : ESMTeam::FutureBass; 
	if (VFX.Find(SourceTeam))
	{
		FFXSystemSpawnParameters NiagaraParams;
		NiagaraParams.WorldContextObject = MyTarget;
		NiagaraParams.SystemTemplate = VFX[SourceTeam];
		NiagaraParams.AttachToComponent = TargetComponent;
		NiagaraParams.Location = TargetLocation;
		NiagaraParams.Rotation = TargetRotation;
		NiagaraParams.LocationType = EAttachLocation::KeepRelativeOffset;
		NiagaraParams.bAutoDestroy = false;
		NiagaraParams.bAutoActivate = true;
		NiagaraParams.PoolingMethod = EPSCPoolMethod::AutoRelease;

		if (TargetComponent)
		{
			if (UNiagaraComponent* VFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttachedWithParams(NiagaraParams))
			{
				VFXComponent->SetAbsolute(false, !bUseRotationAttach, false);
			}
		}
		else
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocationWithParams(NiagaraParams);
		}
	}

	if (SFX.Find(SourceTeam))
	{
		if (TargetComponent)
		{
			UFMODBlueprintStatics::PlayEventAttached(SFX[SourceTeam], TargetComponent, NAME_None, TargetLocation, EAttachLocation::KeepRelativeOffset, false, true, true);
		}
		else
		{
			const FTransform TargetTransform(TargetRotation, TargetLocation);
			UFMODBlueprintStatics::PlayEventAtLocation(MyTarget, SFX[SourceTeam], TargetTransform, true);
		}
	}

	return true;
}
