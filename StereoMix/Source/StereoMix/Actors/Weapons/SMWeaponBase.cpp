// Copyright Studio Surround. All Rights Reserved.


#include "SMWeaponBase.h"


const FName ASMWeaponBase::WeaponMeshComponentName = TEXT("WeaponMeshComponent");

ASMWeaponBase::ASMWeaponBase(const FObjectInitializer& ObjectInitializer)
{
	bReplicates = true;
	bAlwaysRelevant = true;

	SetActorEnableCollision(false);

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	WeaponMeshComponent = CreateDefaultSubobject<UMeshComponent>(ASMWeaponBase::WeaponMeshComponentName);
	if (WeaponMeshComponent)
	{
		WeaponMeshComponent->SetupAttachment(RootComponent);
		WeaponMeshComponent->bReceivesDecals = false;
	}
}

void ASMWeaponBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	OriginalMaterials = WeaponMeshComponent->GetMaterials();
	OriginalOverlayMaterial = WeaponMeshComponent->GetOverlayMaterial();
}
