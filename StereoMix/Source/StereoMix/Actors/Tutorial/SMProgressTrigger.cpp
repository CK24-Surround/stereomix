// Copyright Studio Surround. All Rights Reserved.


#include "SMProgressTrigger.h"

#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "Utilities/SMCollision.h"


ASMProgressTrigger::ASMProgressTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	SetRootComponent(RootSceneComponent);

	ColliderComponent = CreateDefaultSubobject<USphereComponent>(TEXT("ColliderComponent"));
	ColliderComponent->SetupAttachment(RootSceneComponent);
	ColliderComponent->SetCollisionProfileName(SMCollisionProfileName::Trigger);

	VFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("VFXComponent"));
	VFXComponent->SetupAttachment(RootSceneComponent);
	VFXComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
}
