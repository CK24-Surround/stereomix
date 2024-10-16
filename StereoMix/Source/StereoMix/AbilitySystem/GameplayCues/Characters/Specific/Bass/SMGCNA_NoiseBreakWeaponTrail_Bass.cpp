// Copyright Studio Surround. All Rights Reserved.


#include "SMGCNA_NoiseBreakWeaponTrail_Bass.h"

#include "FMODBlueprintStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "FunctionLibraries/SMGameplayCueBlueprintLibrary.h"


bool ASMGCNA_NoiseBreakWeaponTrail_Bass::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	const ASMPlayerCharacterBase* SourceCharacter = Cast<ASMPlayerCharacterBase>(MyTarget);
	USceneComponent* OwnerWeaponMesh = Parameters.TargetAttachComponent.Get();
	if (!SourceCharacter || !OwnerWeaponMesh)
	{
		return false;
	}

	FVector FXOffsetLocation;
	FRotator FXOffsetRotation;
	USMGameplayCueBlueprintLibrary::GetLocationAndRotation(Parameters, FXOffsetLocation, FXOffsetRotation);

	const ESMTeam SourceTeam = SourceCharacter->GetTeam();

	if (FMath::IsNearlyZero(Parameters.RawMagnitude)) // 0이면 캐릭터 아니면 아이템입니다.
	{
		if (VFX.Find(SourceTeam))
		{
			VFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(VFX[SourceTeam], OwnerWeaponMesh, NAME_None, FXOffsetLocation, FXOffsetRotation, EAttachLocation::KeepRelativeOffset, false, true, ENCPoolMethod::ManualRelease);
		}
	}
	else
	{
		if (HealPackVFX)
		{
			VFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(HealPackVFX, OwnerWeaponMesh, NAME_None, FXOffsetLocation, FXOffsetRotation, EAttachLocation::KeepRelativeOffset, false, true, ENCPoolMethod::ManualRelease);
		}
	}

	// if (SFX.Find(SourceTeam))
	// {
	// 	SFXComponent = UFMODBlueprintStatics::PlayEventAttached(SFX[SourceTeam], OwnerWeaponMesh, NAME_None, FXOffsetLocation, EAttachLocation::KeepRelativeOffset, false, true, true);
	// }

	return true;
}

bool ASMGCNA_NoiseBreakWeaponTrail_Bass::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	(void)Super::OnRemove_Implementation(MyTarget, Parameters);

	if (VFXComponent)
	{
		VFXComponent->ReleaseToPool();
		VFXComponent->Deactivate();
		VFXComponent = nullptr;
	}

	// if (SFXComponent)
	// {
	// 	SFXComponent->Deactivate();
	// 	SFXComponent = nullptr;
	// }

	return true;
}
