// Copyright Surround, Inc. All Rights Reserved.


#include "SMItemBase.h"

#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "Utilities/SMCollision.h"

ASMItemBase::ASMItemBase(const FObjectInitializer& ObjectInitializer)
{
	bReplicates = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	ColliderComponent = CreateDefaultSubobject<USphereComponent>(TEXT("ColliderComponent"));
	ColliderComponent->SetupAttachment(RootComponent);
	ColliderComponent->InitSphereRadius(50.0f);

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(ColliderComponent);
	NiagaraComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(ColliderComponent);
	MeshComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
}

void ASMItemBase::ActivateItem(AActor* InActivator)
{
	(void)OnUsedItem.ExecuteIfBound();
}
