// Copyright Studio Surround. All Rights Reserved.


#include "SMGCNA_Neutralize.h"

#include "FMODBlueprintStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/Note/SMNoteBase.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"
#include "Utilities/SMLog.h"

ASMGCNA_Neutralize::ASMGCNA_Neutralize()
{
	for (int32 i = 1; i < static_cast<int32>(ESMTeam::Max); ++i)
	{
		ESMTeam Key = static_cast<ESMTeam>(i);
		LoopVFX.Add(Key, nullptr);
		LoopFloorVFX.Add(Key, nullptr);
		EndVFX.Add(Key, nullptr);
		EndSFX.Add(Key, nullptr);
	}
}

bool ASMGCNA_Neutralize::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	ASMPlayerCharacterBase* SourceCharacter = Cast<ASMPlayerCharacterBase>(MyTarget);
	UWorld* World = SourceCharacter ? SourceCharacter->GetWorld() : nullptr;
	USceneComponent* SourceRoot = SourceCharacter ? SourceCharacter->GetRootComponent() : nullptr;
	ASMNoteBase* SourceNote = SourceCharacter ? SourceCharacter->GetNote() : nullptr;
	USkeletalMeshComponent* SourceNoteMesh = SourceNote ? SourceNote->GetMesh() : nullptr;
	if (!SourceCharacter || !World || !SourceRoot || !SourceNoteMesh)
	{
		return false;
	}

	ESMTeam SourceTeam = USMTeamBlueprintLibrary::GetTeam(MyTarget);

	if (VFX.Find(SourceTeam))
	{
		NET_LOG(SourceCharacter, Warning, TEXT(""));
		UNiagaraFunctionLibrary::SpawnSystemAttached(VFX[SourceTeam], SourceRoot, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, true, ENCPoolMethod::AutoRelease);
	}

	if (SFX.Find(SourceTeam))
	{
		UFMODBlueprintStatics::PlayEventAttached(SFX[SourceTeam], SourceRoot, NAME_None, FVector::ZeroVector, EAttachLocation::KeepRelativeOffset, false, true, true);
	}

	if (LoopVFX.Find(SourceTeam))
	{
		const FName SoketName = TEXT("PRoot");
		LoopVFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(LoopVFX[SourceTeam], SourceNoteMesh, SoketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, true, ENCPoolMethod::ManualRelease);
	}

	if (LoopFloorVFX.Find(SourceTeam))
	{
		UCapsuleComponent* SourceCapsule = SourceCharacter->GetCapsuleComponent();
		const float SourceCapsuleHalfHeight = SourceCapsule ? SourceCapsule->GetScaledCapsuleHalfHeight() : 0.0f;
		const FVector Offset(0.0, 0.0, -SourceCapsuleHalfHeight);
		LoopVFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(LoopFloorVFX[SourceTeam], SourceNoteMesh, NAME_None, Offset, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, true, ENCPoolMethod::ManualRelease);
	}

	return true;
}

bool ASMGCNA_Neutralize::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	(void)Super::OnRemove_Implementation(MyTarget, Parameters);

	if (LoopVFXComponent)
	{
		LoopVFXComponent->Deactivate();
		LoopVFXComponent->ReleaseToPool();
		LoopVFXComponent = nullptr;
	}

	if (LoopFloorVFXComponent)
	{
		LoopFloorVFXComponent->Deactivate();
		LoopFloorVFXComponent->ReleaseToPool();
		LoopFloorVFXComponent = nullptr;
	}

	ASMPlayerCharacterBase* SourceCharacter = Cast<ASMPlayerCharacterBase>(MyTarget);
	UWorld* World = SourceCharacter ? SourceCharacter->GetWorld() : nullptr;
	USceneComponent* SourceRoot = SourceCharacter ? SourceCharacter->GetRootComponent() : nullptr;
	ASMNoteBase* SourceNote = SourceCharacter ? SourceCharacter->GetNote() : nullptr;
	USkeletalMeshComponent* SourceNoteMesh = SourceNote ? SourceNote->GetMesh() : nullptr;
	if (!SourceCharacter || !World || !SourceRoot || !SourceNoteMesh)
	{
		return false;
	}

	ESMTeam SourceTeam = USMTeamBlueprintLibrary::GetTeam(MyTarget);

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
