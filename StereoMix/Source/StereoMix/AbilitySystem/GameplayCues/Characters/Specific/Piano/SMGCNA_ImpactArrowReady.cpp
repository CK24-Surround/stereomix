// Copyright Studio Surround. All Rights Reserved.


#include "SMGCNA_ImpactArrowReady.h"

#include "FMODBlueprintStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Actors/Character/Player/SMPianoCharacter.h"
#include "Actors/Weapons/SMWeaponBase.h"


ASMGCNA_ImpactArrowReady::ASMGCNA_ImpactArrowReady()
{
	PrimaryActorTick.bCanEverTick = false;
}

bool ASMGCNA_ImpactArrowReady::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	ASMPianoCharacter* SourceCharacter = Cast<ASMPianoCharacter>(MyTarget);
	const ASMWeaponBase* sourceWeapon = SourceCharacter ? SourceCharacter->GetWeapon() : nullptr;
	UMeshComponent* BowMesh = sourceWeapon ? sourceWeapon->GetWeaponMeshComponent() : nullptr;
	if (!SourceCharacter || !BowMesh)
	{
		return false;
	}

	const ESMTeam SourceTeam = SourceCharacter->GetTeam();
	const FVector WeaponOffset(0.0, 0.0, 20.0);

	if (VFX.Find(SourceTeam))
	{
		VFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(VFX[SourceTeam], BowMesh, NAME_None, WeaponOffset, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, true, ENCPoolMethod::ManualRelease);
	}

	if (SFX.Find(SourceTeam))
	{
		SFXComponent = UFMODBlueprintStatics::PlayEventAttached(SFX[SourceTeam], BowMesh, NAME_None, WeaponOffset, EAttachLocation::KeepRelativeOffset, false, true, true);
	}

	SourceCharacter->SetWeaponVFXEnabled(false);

	return true;
}

bool ASMGCNA_ImpactArrowReady::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	(void)Super::OnRemove_Implementation(MyTarget, Parameters);

	const bool bIsSuccess = FMath::IsNearlyEqual(Parameters.RawMagnitude, 1.0f);

	if (VFXComponent)
	{
		VFXComponent->Deactivate();
		VFXComponent->ReleaseToPool();
		VFXComponent = nullptr;
	}

	if (SFXComponent)
	{
		const FName ParameterName = TEXT("ShotSuccess");
		const float ParameterValue = bIsSuccess ? 0.0f : 1.0f;
		SFXComponent->SetParameter(ParameterName, ParameterValue);
		SFXComponent->Stop();
		SFXComponent = nullptr;
	}

	// 노트 상태 즉 무력화 상태가 아닌 경우만 이펙트를 다시 활성화합니다.
	ASMPianoCharacter* SourceCharacter = Cast<ASMPianoCharacter>(MyTarget);
	if (SourceCharacter ? !SourceCharacter->IsNoteState() : false)
	{
		SourceCharacter->SetWeaponVFXEnabled(true);
	}

	return true;
}
