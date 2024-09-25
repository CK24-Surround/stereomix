// Copyright Surround, Inc. All Rights Reserved.


#include "SMCatchableItem.h"

#include "Utilities/SMCollision.h"


ASMCatchableItem::ASMCatchableItem()
{
	// StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	// StaticMeshComponent->SetupAttachment(SceneComponent);
	// StaticMeshComponent->SetCollisionProfileName(SMCollisionProfileName::CatchableItem);
}

void ASMCatchableItem::ActivateItem(AActor* InActivator)
{
	Super::ActivateItem(InActivator);

	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
}
