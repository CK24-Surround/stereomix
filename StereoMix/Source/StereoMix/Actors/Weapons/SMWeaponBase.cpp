// Copyright Studio Surround. All Rights Reserved.


#include "SMWeaponBase.h"

#include "Utilities/SMCollision.h"


const FName ASMWeaponBase::WeaponMeshComponentName = TEXT("WeaponMeshComponent");

ASMWeaponBase::ASMWeaponBase(const FObjectInitializer& ObjectInitializer)
{
	bReplicates = true;
	bAlwaysRelevant = true;

	SetActorEnableCollision(false);

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	// 추측하건데 abstract 컴포넌트는 하위 클래스에서 정의 되기전까지 인스턴스화가 불가능해 nullptr인 경우가 생겨 null 체크를 해줘야하는 것 같습니다.
	if (WeaponMeshComponent = CreateDefaultSubobject<UMeshComponent>(ASMWeaponBase::WeaponMeshComponentName); WeaponMeshComponent)
	{
		WeaponMeshComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
		WeaponMeshComponent->SetupAttachment(RootComponent);
		WeaponMeshComponent->bRenderCustomDepth = true;
		WeaponMeshComponent->bReceivesDecals = false;
	}
}

void ASMWeaponBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	OriginalMaterials = WeaponMeshComponent->GetMaterials();
	OriginalOverlayMaterial = WeaponMeshComponent->GetOverlayMaterial();
}
