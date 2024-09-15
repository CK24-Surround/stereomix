// Copyright Studio Surround. All Rights Reserved.


#include "SMWeaponBase.h"


const FName ASMWeaponBase::WeaponMeshComponentName = TEXT("WeaponMeshComponent");

ASMWeaponBase::ASMWeaponBase(const FObjectInitializer& ObjectInitializer)
{
	bReplicates = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	WeaponMeshComponent = CreateDefaultSubobject<UMeshComponent>(ASMWeaponBase::WeaponMeshComponentName);
	if (WeaponMeshComponent)
	{
		WeaponMeshComponent->SetupAttachment(RootComponent);
	}
}
