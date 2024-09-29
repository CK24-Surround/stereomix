// Copyright Studio Surround. All Rights Reserved.


#include "SMGCNA_Neutralize.h"

#include "FMODBlueprintStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"

ASMGCNA_Neutralize::ASMGCNA_Neutralize()
{
	for (int32 i = 1; i < static_cast<int32>(ESMTeam::Max); ++i)
	{
		ESMTeam Key = static_cast<ESMTeam>(i);
		EndVFX.Add(Key, nullptr);
		EndSFX.Add(Key, nullptr);
	}
}

bool ASMGCNA_Neutralize::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	UWorld* World = MyTarget ? MyTarget->GetWorld() : nullptr;
	USceneComponent* TargetComponent = Parameters.TargetAttachComponent.Get();
	if (!World || !TargetComponent)
	{
		return false;
	}

	ESMTeam SourceTeam = USMTeamBlueprintLibrary::GetTeam(MyTarget);

	if (VFX.Find(SourceTeam))
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(VFX[SourceTeam], TargetComponent, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, true, ENCPoolMethod::AutoRelease);
	}

	if (SFX.Find(SourceTeam))
	{
		UFMODBlueprintStatics::PlayEventAttached(SFX[SourceTeam], TargetComponent, NAME_None, FVector::ZeroVector, EAttachLocation::KeepRelativeOffset, false, true, true);
	}

	return true;
}

bool ASMGCNA_Neutralize::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	(void)Super::OnRemove_Implementation(MyTarget, Parameters);

	UWorld* World = MyTarget ? MyTarget->GetWorld() : nullptr;
	USceneComponent* TargetComponent = Parameters.TargetAttachComponent.Get();
	if (!World || !TargetComponent)
	{
		return false;
	}

	ESMTeam SourceTeam = USMTeamBlueprintLibrary::GetTeam(MyTarget);

	if (EndVFX.Find(SourceTeam))
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(EndVFX[SourceTeam], TargetComponent, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, true, ENCPoolMethod::AutoRelease);
	}

	if (EndSFX.Find(SourceTeam))
	{
		UFMODBlueprintStatics::PlayEventAttached(EndSFX[SourceTeam], TargetComponent, NAME_None, FVector::ZeroVector, EAttachLocation::KeepRelativeOffset, false, true, true);
	}

	return true;
}
