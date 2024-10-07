// Copyright Studio Surround. All Rights Reserved.


#include "SMGCNA_HoldWeapon.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Actors/Weapons/SMWeaponBase.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"


ASMGCNA_HoldWeapon::ASMGCNA_HoldWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
}

bool ASMGCNA_HoldWeapon::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	const ASMPlayerCharacterBase* SourceCharacter = Cast<ASMPlayerCharacterBase>(MyTarget);
	const ASMWeaponBase* Weapon = SourceCharacter ? SourceCharacter->GetWeapon() : nullptr;
	UMeshComponent* WeaponMesh = Weapon ? Weapon->GetWeaponMeshComponent() : nullptr;
	if (!MyTarget || !WeaponMesh)
	{
		return false;
	}

	const ESMTeam SourceTeam = SourceCharacter->GetTeam();

	if (VFX.Contains(SourceTeam))
	{
		VFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(VFX[SourceTeam], WeaponMesh, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, true, ENCPoolMethod::ManualRelease);
	}

	if (OverlayMaterial.Contains(SourceTeam))
	{
		WeaponMesh->SetOverlayMaterial(OverlayMaterial[SourceTeam]);
	}

	return true;
}

bool ASMGCNA_HoldWeapon::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	(void)Super::OnRemove_Implementation(MyTarget, Parameters);

	if (VFXComponent)
	{
		VFXComponent->Deactivate();
		VFXComponent->ReleaseToPool();
		VFXComponent = nullptr;
	}

	const ASMPlayerCharacterBase* SourceCharacter = Cast<ASMPlayerCharacterBase>(MyTarget);
	const ASMWeaponBase* Weapon = SourceCharacter ? SourceCharacter->GetWeapon() : nullptr;
	UMeshComponent* WeaponMesh = Weapon ? Weapon->GetWeaponMeshComponent() : nullptr;
	if (!MyTarget || !WeaponMesh)
	{
		return false;
	}

	WeaponMesh->SetOverlayMaterial(Weapon->GetOriginalOverlayMaterial());

	return true;
}
