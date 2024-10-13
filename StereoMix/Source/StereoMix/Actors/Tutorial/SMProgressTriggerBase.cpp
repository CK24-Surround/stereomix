// Copyright Studio Surround. All Rights Reserved.


#include "SMProgressTriggerBase.h"

#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Utilities/SMCollision.h"


ASMProgressTriggerBase::ASMProgressTriggerBase()
{
	PrimaryActorTick.bCanEverTick = false;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	SetRootComponent(RootSceneComponent);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("ColliderComponent"));
	SphereComponent->SetupAttachment(RootSceneComponent);
	SphereComponent->SetCollisionProfileName(SMCollisionProfileName::PlayerTrigger);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(RootSceneComponent);
	BoxComponent->SetCollisionProfileName(SMCollisionProfileName::PlayerTrigger);

	VFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("VFXComponent"));
	VFXComponent->SetupAttachment(RootSceneComponent);
	VFXComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
}
