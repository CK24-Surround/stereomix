// Copyright Studio Surround. All Rights Reserved.


#include "SMHoldableItemBase.h"

#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "HoldInteraction/SMHoldInteractionComponent.h"
#include "Utilities/SMCollision.h"

const FName ASMHoldableItemBase::HICName = TEXT("HIC");

ASMHoldableItemBase::ASMHoldableItemBase(const FObjectInitializer& ObjectInitializer)
{
	Super::SetReplicateMovement(true);

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	ColliderComponent = CreateDefaultSubobject<USphereComponent>(TEXT("ColliderComponent"));
	ColliderComponent->SetupAttachment(RootComponent);
	ColliderComponent->SetCollisionProfileName(SMCollisionProfileName::HoldableItem);
	ColliderComponent->InitSphereRadius(50.0f);

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(ColliderComponent);
	NiagaraComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(ColliderComponent);
	MeshComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);

	HIC = CreateDefaultSubobject<USMHoldInteractionComponent>(HICName);
}
