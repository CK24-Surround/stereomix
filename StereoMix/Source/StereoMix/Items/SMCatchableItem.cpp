// Copyright Surround, Inc. All Rights Reserved.


#include "SMCatchableItem.h"

#include "Utilities/SMCollision.h"


ASMCatchableItem::ASMCatchableItem()
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	StaticMeshComponent->SetupAttachment(SceneComponent);
	StaticMeshComponent->SetCollisionProfileName(SMCollisionProfileName::CatchableItem);
}
