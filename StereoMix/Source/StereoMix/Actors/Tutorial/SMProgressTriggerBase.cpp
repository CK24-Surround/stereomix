// Copyright Studio Surround. All Rights Reserved.


#include "SMProgressTriggerBase.h"

#include "NiagaraComponent.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Utilities/SMCollision.h"


ASMProgressTriggerBase::ASMProgressTriggerBase()
{
	PrimaryActorTick.bCanEverTick = true;

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

void ASMProgressTriggerBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 오버랩되고 있던 중이라도 OnActorBeginOverlap를 브로드캐스트하도록합니다.
	TArray<AActor*> OverlappedActors;
	GetOverlappingActors(OverlappedActors, ASMPlayerCharacterBase::StaticClass());
	for (AActor* OverlappedActor : OverlappedActors)
	{
		OnActorBeginOverlap.Broadcast(this, OverlappedActor);
	}
}
