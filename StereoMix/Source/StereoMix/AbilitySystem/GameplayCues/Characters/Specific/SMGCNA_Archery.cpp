// Copyright Studio Surround. All Rights Reserved.


#include "SMGCNA_Archery.h"

#include "FMODBlueprintStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/Player/SMPianoCharacter.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"


ASMGCNA_Archery::ASMGCNA_Archery()
{
	PrimaryActorTick.bCanEverTick = true;
}

bool ASMGCNA_Archery::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	ASMPianoCharacter* SourceCharacter = Cast<ASMPianoCharacter>(MyTarget);
	USceneComponent* BowMesh = Parameters.TargetAttachComponent.Get();
	if (!SourceCharacter || !BowMesh)
	{
		return false;
	}

	const ESMTeam SourceTeam = SourceCharacter->GetTeam();

	if (VFX.Find(SourceTeam))
	{
		VFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(VFX[SourceTeam], BowMesh, NAME_None, Offset, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, true, ENCPoolMethod::ManualRelease);
	}

	if (SFX.Find(SourceTeam))
	{
		SFXComponent = UFMODBlueprintStatics::PlayEventAttached(SFX[SourceTeam], BowMesh, NAME_None, Offset, EAttachLocation::KeepRelativeOffset, false, true, true);
	}

	SourceCharacter->SetWeaponVFXEnabled(false);

	return true;
}

bool ASMGCNA_Archery::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	(void)Super::OnRemove_Implementation(MyTarget, Parameters);

	const int32 ChargedLevel = static_cast<int32>(Parameters.RawMagnitude);

	if (VFXComponent)
	{
		VFXComponent->Deactivate();
		VFXComponent->ReleaseToPool();
		VFXComponent = nullptr;
	}

	if (SFXComponent)
	{
		const FName ParameterName = TEXT("ArrowCharge");
		const float ParameterValue = ChargedLevel >= 0 ? ChargedLevel : 0.0f;
		SFXComponent->SetParameter(ParameterName, ParameterValue);

		SFXComponent->Stop();
		SFXComponent = nullptr;
	}

	if (ASMPianoCharacter* SourceCharacter = Cast<ASMPianoCharacter>(MyTarget))
	{
		// 노트 상태 즉 무력화 상태가 아닌 경우만 이펙트를 다시 활성화합니다.
		if (!SourceCharacter->IsNoteState())
		{
			SourceCharacter->SetWeaponVFXEnabled(true);
		}
	}

	return true;
}
