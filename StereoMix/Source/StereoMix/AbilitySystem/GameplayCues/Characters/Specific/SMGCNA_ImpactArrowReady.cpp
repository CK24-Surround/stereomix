// Copyright Studio Surround. All Rights Reserved.


#include "SMGCNA_ImpactArrowReady.h"

#include "FMODBlueprintStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Actors/Character/Player/SMPianoCharacter.h"
#include "FunctionLibraries/SMGameplayCueBlueprintLibrary.h"


ASMGCNA_ImpactArrowReady::ASMGCNA_ImpactArrowReady()
{
	PrimaryActorTick.bCanEverTick = false;
}

bool ASMGCNA_ImpactArrowReady::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	ASMPianoCharacter* SourceCharacter = Cast<ASMPianoCharacter>(MyTarget);
	USceneComponent* BowMesh = Parameters.TargetAttachComponent.Get();
	if (!SourceCharacter || !BowMesh)
	{
		return false;
	}

	FVector SourceLocation;
	FRotator SourceRotation;
	USMGameplayCueBlueprintLibrary::GetLocationAndRotation(Parameters, SourceLocation, SourceRotation);

	const ESMTeam SourceTeam = SourceCharacter->GetTeam();

	if (VFX.Find(SourceTeam))
	{
		VFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(VFX[SourceTeam], BowMesh, NAME_None, SourceLocation, SourceRotation, EAttachLocation::KeepRelativeOffset, false, true, ENCPoolMethod::ManualRelease);
	}

	if (SFX.Find(SourceTeam))
	{
		SFXComponent = UFMODBlueprintStatics::PlayEventAttached(SFX[SourceTeam], BowMesh, NAME_None, SourceLocation, EAttachLocation::KeepRelativeOffset, false, true, true);
	}

	SourceCharacter->SetWeaponVFXEnabled(false);

	return true;
}

bool ASMGCNA_ImpactArrowReady::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	(void)Super::OnRemove_Implementation(MyTarget, Parameters);

	if (VFXComponent)
	{
		VFXComponent->Deactivate();
		VFXComponent->ReleaseToPool();
		VFXComponent = nullptr;
	}

	// 노트 상태 즉 무력화 상태가 아닌 경우만 이펙트를 다시 활성화합니다.
	ASMPianoCharacter* SourceCharacter = Cast<ASMPianoCharacter>(MyTarget);
	if (SourceCharacter ? !SourceCharacter->IsNoteState() : false)
	{
		SourceCharacter->SetWeaponVFXEnabled(true);
	}

	return true;
}
