// Copyright Studio Surround. All Rights Reserved.


#include "SMGCNA_Immune.h"

#include "FMODBlueprintStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Actors/Weapons/SMWeaponBase.h"
#include "Camera/CameraComponent.h"
#include "Utilities/SMLog.h"


ASMGCNA_Immune::ASMGCNA_Immune()
{
	for (int32 i = 1; i < static_cast<int32>(ESMTeam::Max); ++i)
	{
		ESMTeam Key = static_cast<ESMTeam>(i);
		EndVFX.Add(Key, nullptr);
		ScreenVFX.Add(Key, nullptr);
	}
}

bool ASMGCNA_Immune::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	const ASMPlayerCharacterBase* SourceCharacter = Cast<ASMPlayerCharacterBase>(MyTarget);
	USkeletalMeshComponent* SourceMesh = SourceCharacter ? SourceCharacter->GetMesh() : nullptr;
	const ASMWeaponBase* SourceWeapon = SourceCharacter ? SourceCharacter->GetWeapon() : nullptr;
	UMeshComponent* SourceWeaponMesh = SourceWeapon ? SourceWeapon->GetWeaponMeshComponent() : nullptr;
	if (!SourceMesh || !SourceWeaponMesh)
	{
		return false;
	}

	const ESMTeam SourceTeam = SourceCharacter->GetTeam();

	if (VFX.Contains(SourceTeam))
	{
		VFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(VFX[SourceTeam], SourceCharacter->GetRootComponent(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, true, ENCPoolMethod::ManualRelease);
	}

	if (ImmuneMaterial.Contains(SourceTeam) && ImmuneOverlayMaterial.Find(SourceTeam))
	{
		for (int32 i = 0; i < SourceMesh->GetNumMaterials(); ++i)
		{
			SourceMesh->SetMaterial(i, ImmuneMaterial[SourceTeam]);
		}

		for (int32 i = 0; i < SourceWeaponMesh->GetNumMaterials(); ++i)
		{
			SourceWeaponMesh->SetMaterial(i, ImmuneMaterial[SourceTeam]);
		}

		SourceMesh->SetOverlayMaterial(ImmuneOverlayMaterial[SourceTeam]);
		SourceWeaponMesh->SetOverlayMaterial(ImmuneOverlayMaterial[SourceTeam]);
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
			SFXComponent = UFMODBlueprintStatics::PlayEventAttached(SFX[SourceTeam], SourceCharacter->GetRootComponent(), NAME_None, FVector::ZeroVector, EAttachLocation::KeepRelativeOffset, false, true, true);
		}
	}

	return true;
}

bool ASMGCNA_Immune::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	(void)Super::OnRemove_Implementation(MyTarget, Parameters);

	if (VFXComponent)
	{
		VFXComponent->Deactivate();
		VFXComponent->ReleaseToPool();
		VFXComponent = nullptr;
	}

	if (ScreenVFXComponent)
	{
		ScreenVFXComponent->Deactivate();
		ScreenVFXComponent->ReleaseToPool();
		ScreenVFXComponent = nullptr;
	}

	if (SFXComponent)
	{
		SFXComponent->Stop();
		SFXComponent = nullptr;
	}

	ASMPlayerCharacterBase* SourceCharacter = Cast<ASMPlayerCharacterBase>(MyTarget);
	USkeletalMeshComponent* SourceMesh = SourceCharacter ? SourceCharacter->GetMesh() : nullptr;
	const ASMWeaponBase* SourceWeapon = SourceCharacter ? SourceCharacter->GetWeapon() : nullptr;
	UMeshComponent* SourceWeaponMesh = SourceWeapon ? SourceWeapon->GetWeaponMeshComponent() : nullptr;
	if (!SourceMesh || !SourceWeaponMesh)
	{
		return false;
	}

	TArray<UMaterialInterface*> OriginalMaterials = SourceCharacter->GetOriginalMaterials();
	UMaterialInterface* OriginalOverlayMaterial = SourceCharacter->GetOriginalOverlayMaterial();
	for (int32 i = 0; i < SourceMesh->GetNumMaterials(); ++i)
	{
		SourceMesh->SetMaterial(i, OriginalMaterials[i]);
	}

	SourceMesh->SetOverlayMaterial(OriginalOverlayMaterial);

	TArray<UMaterialInterface*> WeaponOriginalMaterials = SourceWeapon->GetOriginalMaterials();
	UMaterialInterface* WeaponOriginalOverlayMaterial = SourceWeapon->GetOriginalOverlayMaterial();
	for (int32 i = 0; i < SourceWeaponMesh->GetNumMaterials(); ++i)
	{
		SourceWeaponMesh->SetMaterial(i, WeaponOriginalMaterials[i]);
	}

	SourceWeaponMesh->SetOverlayMaterial(WeaponOriginalOverlayMaterial);

	const ESMTeam SourceTeam = SourceCharacter->GetTeam();

	if (EndVFX.Contains(SourceTeam))
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(EndVFX[SourceTeam], SourceCharacter->GetRootComponent(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, true, ENCPoolMethod::AutoRelease);
	}

	return true;
}
