// Copyright Studio Surround. All Rights Reserved.


#include "SMGCN_HitFlash.h"

#include "Actors/Character/SMCharacterBase.h"
#include "Actors/Weapons/SMWeaponBase.h"
#include "Utilities/SMLog.h"


bool USMGCN_HitFlash::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	UMeshComponent* OwnerMesh = Cast<UMeshComponent>(Parameters.TargetAttachComponent);
	const UWorld* World = MyTarget ? MyTarget->GetWorld() : nullptr;
	if (!OwnerMesh || !World)
	{
		return false;
	}

	const ASMCharacterBase* OwnerCharacter = Cast<ASMCharacterBase>(MyTarget);
	ESMShaderStencil DefaultShaderStencil = OwnerCharacter ? OwnerCharacter->GetDefaultShaderStencil() : ESMShaderStencil::NonOutline;

	OwnerMesh->SetCustomDepthStencilValue(static_cast<int32>(ESMShaderStencil::Hit));

	const ASMWeaponBase* OwnerWeapon = OwnerCharacter->GetWeapon();
	UMeshComponent* OwnerWeaponMesh = OwnerWeapon ? OwnerWeapon->GetWeaponMeshComponent() : nullptr;
	if (OwnerWeaponMesh)
	{
		OwnerWeaponMesh->SetCustomDepthStencilValue(static_cast<int32>(ESMShaderStencil::Hit));
	}

	auto RecoverStencilValue = [OwnerMeshWeakPtr = MakeWeakObjectPtr(OwnerMesh), CopiedDefaultShaderStencil = static_cast<int32>(DefaultShaderStencil), OwnerWeaponMeshWeakPtr = MakeWeakObjectPtr(OwnerWeaponMesh)] {
		if (OwnerMeshWeakPtr.IsValid())
		{
			OwnerMeshWeakPtr->SetCustomDepthStencilValue(CopiedDefaultShaderStencil);
		}

		if (OwnerWeaponMeshWeakPtr.IsValid())
		{
			OwnerWeaponMeshWeakPtr->SetCustomDepthStencilValue(CopiedDefaultShaderStencil);
		}
	};

	FTimerHandle TimerHandle;
	World->GetTimerManager().SetTimer(TimerHandle, RecoverStencilValue, 0.075f, false);

	return false;
}
