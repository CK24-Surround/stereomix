// Copyright Studio Surround. All Rights Reserved.


#include "SMGCNA_Buff.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Camera/CameraComponent.h"


ASMGCNA_Buff::ASMGCNA_Buff()
{
	PrimaryActorTick.bCanEverTick = false;
}

bool ASMGCNA_Buff::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	const ASMPlayerCharacterBase* OwnerCharacter = Cast<ASMPlayerCharacterBase>(MyTarget);
	if (!OwnerCharacter)
	{
		return false;
	}

	const ESMTeam OwnerTeam = OwnerCharacter->GetTeam();

	if (OwnerCharacter->IsLocallyControlled())
	{
		UCameraComponent* CameraComponent = OwnerCharacter->GetComponentByClass<UCameraComponent>();
		UNiagaraSystem* CachedVFX = VFX.Contains(OwnerTeam) ? VFX[OwnerTeam] : nullptr;
		if (CameraComponent && CachedVFX)
		{
			UNiagaraFunctionLibrary::SpawnSystemAttached(CachedVFX, CameraComponent, NAME_None, FVector(300.0, 0.0, 0.0), FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, true, ENCPoolMethod::AutoRelease);
		}
	}

	USceneComponent* OwnerRoot = OwnerCharacter->GetRootComponent();
	UNiagaraSystem* CachedLoopVFX = LoopVFX.Contains(OwnerTeam) ? LoopVFX[OwnerTeam] : nullptr;
	if (OwnerRoot && CachedLoopVFX)
	{
		if (LoopVFXComponent)
		{
			LoopVFXComponent->ReleaseToPool();
			LoopVFXComponent->Deactivate();
			LoopVFXComponent = nullptr;
		}

		if (LoopVFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(CachedLoopVFX, OwnerRoot, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, true, ENCPoolMethod::ManualRelease); LoopVFXComponent)
		{
			LoopVFXComponent->SetAbsolute(false, true, true);
		}
	}

	USkeletalMeshComponent* OwnerMesh = OwnerCharacter->GetMesh();
	UMaterialInterface* CachedOverlayMaterial = OverlayMaterial.Contains(OwnerTeam) ? OverlayMaterial[OwnerTeam] : nullptr;
	if (OwnerMesh && CachedOverlayMaterial)
	{
		OwnerMesh->SetOverlayMaterial(CachedOverlayMaterial);
	}

	return true;
}

bool ASMGCNA_Buff::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	(void)Super::OnRemove_Implementation(MyTarget, Parameters);

	if (LoopVFXComponent)
	{
		LoopVFXComponent->ReleaseToPool();
		LoopVFXComponent->Deactivate();
		LoopVFXComponent = nullptr;
	}

	const ASMPlayerCharacterBase* OwnerCharacter = Cast<ASMPlayerCharacterBase>(MyTarget);
	if (!OwnerCharacter)
	{
		return false;
	}

	const ESMTeam OwnerTeam = OwnerCharacter->GetTeam();

	if (OverlayMaterial.Contains(OwnerTeam))
	{
		if (USkeletalMeshComponent* OwnerMesh = OwnerCharacter->GetMesh())
		{
			OwnerMesh->SetOverlayMaterial(OwnerCharacter->GetOriginalOverlayMaterial());
		}
	}

	return true;
}
