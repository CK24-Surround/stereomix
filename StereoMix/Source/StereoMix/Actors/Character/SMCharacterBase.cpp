// Copyright Surround, Inc. All Rights Reserved.


#include "SMCharacterBase.h"

#include "Actors/Weapons/SMWeaponBase.h"
#include "Net/UnrealNetwork.h"


ASMCharacterBase::ASMCharacterBase(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASMCharacterBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Weapon);
}

void ASMCharacterBase::RecalculateDefaultStencil()
{
	DefaultShaderStencil = ESMShaderStencil::NonOutline;
}

void ASMCharacterBase::ApplyDefaultStencil()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (USkeletalMeshComponent* CachedMeshComponent = GetMesh())
	{
		CachedMeshComponent->SetCustomDepthStencilValue(static_cast<int32>(DefaultShaderStencil));
	}

	const ASMWeaponBase* CachedWeapon = GetWeapon();
	if (UMeshComponent* WeaponMesh = CachedWeapon ? CachedWeapon->GetWeaponMeshComponent() : nullptr)
	{
		WeaponMesh->SetCustomDepthStencilValue(static_cast<int32>(DefaultShaderStencil));
	}
}

void ASMCharacterBase::OnRep_Weapon()
{
	ApplyDefaultStencil();
}
