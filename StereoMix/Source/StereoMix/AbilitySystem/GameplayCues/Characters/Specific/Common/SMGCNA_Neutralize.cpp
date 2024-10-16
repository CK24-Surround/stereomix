// Copyright Studio Surround. All Rights Reserved.


#include "SMGCNA_Neutralize.h"

#include "FMODBlueprintStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Actors/Notes/SMNoteBase.h"
#include "Camera/CameraComponent.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"
#include "Utilities/SMLog.h"

ASMGCNA_Neutralize::ASMGCNA_Neutralize()
{
	for (int32 i = 1; i < static_cast<int32>(ESMTeam::Max); ++i)
	{
		ESMTeam Key = static_cast<ESMTeam>(i);
		EndVFX.Add(Key, nullptr);
		ScreenVFX.Add(Key, nullptr);
		StartSFX.Add(Key, nullptr);
		EndSFX.Add(Key, nullptr);
	}
}

bool ASMGCNA_Neutralize::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	ASMPlayerCharacterBase* SourceCharacter = Cast<ASMPlayerCharacterBase>(MyTarget);
	USceneComponent* SourceRoot = SourceCharacter ? SourceCharacter->GetRootComponent() : nullptr;
	ASMNoteBase* SourceNote = SourceCharacter ? SourceCharacter->GetNote() : nullptr;
	const USkeletalMeshComponent* SourceNoteMesh = SourceNote ? SourceNote->GetMesh() : nullptr;
	if (!SourceCharacter || !SourceRoot || !SourceNoteMesh)
	{
		return false;
	}

	const ESMTeam SourceTeam = USMTeamBlueprintLibrary::GetTeam(MyTarget);

	if (VFX.Contains(SourceTeam))
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(VFX[SourceTeam], SourceRoot, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, true, ENCPoolMethod::AutoRelease);
	}

	if (StartSFX.Contains(SourceTeam))
	{
		UFMODBlueprintStatics::PlayEventAttached(StartSFX[SourceTeam], SourceRoot, NAME_None, FVector::ZeroVector, EAttachLocation::KeepRelativeOffset, false, true, true);
	}

	if (SourceCharacter->IsLocallyControlled())
	{
		if (ScreenVFX.Contains(SourceTeam))
		{
			if (UCameraComponent* SourceCamera = SourceCharacter->GetComponentByClass<UCameraComponent>())
			{
				ScreenVFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(ScreenVFX[SourceTeam], SourceCamera, NAME_None, FVector(300.0, 0.0, 0.0), FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, true, ENCPoolMethod::ManualRelease);
			}
		}

		if (SFX.Contains(SourceTeam))
		{
			SFXComponent = UFMODBlueprintStatics::PlayEventAttached(SFX[SourceTeam], SourceRoot, NAME_None, FVector::ZeroVector, EAttachLocation::KeepRelativeOffset, false, true, true);
		}
	}

	return true;
}

bool ASMGCNA_Neutralize::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	(void)Super::OnRemove_Implementation(MyTarget, Parameters);

	if (ScreenVFXComponent)
	{
		ScreenVFXComponent->Deactivate();
		ScreenVFXComponent->ReleaseToPool();
		ScreenVFXComponent = nullptr;
	}

	if (SFXComponent)
	{
		SFXComponent->Deactivate();
		SFXComponent = nullptr;
	}

	ASMPlayerCharacterBase* SourceCharacter = Cast<ASMPlayerCharacterBase>(MyTarget);
	USceneComponent* SourceRoot = SourceCharacter ? SourceCharacter->GetRootComponent() : nullptr;
	ASMNoteBase* SourceNote = SourceCharacter ? SourceCharacter->GetNote() : nullptr;
	const USkeletalMeshComponent* SourceNoteMesh = SourceNote ? SourceNote->GetMesh() : nullptr;
	if (!SourceCharacter || !SourceRoot || !SourceNoteMesh)
	{
		return false;
	}

	const ESMTeam SourceTeam = USMTeamBlueprintLibrary::GetTeam(MyTarget);

	if (EndVFX.Find(SourceTeam))
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(EndVFX[SourceTeam], SourceRoot, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, true, ENCPoolMethod::AutoRelease);
	}

	if (EndSFX.Find(SourceTeam))
	{
		UFMODBlueprintStatics::PlayEventAttached(EndSFX[SourceTeam], SourceRoot, NAME_None, FVector::ZeroVector, EAttachLocation::KeepRelativeOffset, false, true, true);
	}

	return true;
}
