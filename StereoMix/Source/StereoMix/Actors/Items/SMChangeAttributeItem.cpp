// Copyright Surround, Inc. All Rights Reserved.


#include "SMChangeAttributeItem.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "Utilities/SMCollision.h"

ASMChangeAttributeItem::ASMChangeAttributeItem()
{
	bReplicates = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	SpawnerMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpawnerMeshComponent"));
	SpawnerMeshComponent->SetupAttachment(SceneComponent);
	SpawnerMeshComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(SpawnerMeshComponent);
	SphereComponent->SetCollisionProfileName(SMCollisionProfileName::HealPack);

	ItemMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	ItemMeshComponent->SetupAttachment(SphereComponent);
	ItemMeshComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
}

void ASMChangeAttributeItem::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 클라이언트에서는 충돌이 일어나지 않도록 합니다.
	if (!HasAuthority())
	{
		SetActorEnableCollision(false);
	}
}

void ASMChangeAttributeItem::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
	{
		TargetASC->BP_ApplyGameplayEffectToSelf(GE, 1.0f, TargetASC->MakeEffectContext());
		MulticastRPCSetHidden(true);
		SetActorEnableCollision(false);

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ASMChangeAttributeItem::HealRespawnTimerCallback, RespawnTime);
	}
}

void ASMChangeAttributeItem::HealRespawnTimerCallback()
{
	MulticastRPCSetHidden(false);
	SetActorEnableCollision(true);
}

void ASMChangeAttributeItem::MulticastRPCSetHidden_Implementation(bool bNewHidden)
{
	if (!HasAuthority())
	{
		ItemMeshComponent->SetHiddenInGame(bNewHidden);
	}
}
