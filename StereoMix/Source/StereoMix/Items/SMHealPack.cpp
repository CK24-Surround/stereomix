// Fill out your copyright notice in the Description page of Project Settings.


#include "SMHealPack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"

ASMHealPack::ASMHealPack()
{
	bReplicates = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(SceneComponent);
	SphereComponent->SetCollisionProfileName(SMCollisionProfileName::HealPack);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(SphereComponent);
	MeshComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
}

void ASMHealPack::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 클라이언트에서는 충돌이 일어나지 않도록 합니다.
	if (!HasAuthority())
	{
		SetActorEnableCollision(false);
	}
}

void ASMHealPack::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
	if (ensure(TargetASC))
	{
		NET_LOG(this, Warning, TEXT("힐팩 오버랩"));
		TargetASC->BP_ApplyGameplayEffectToSelf(HealGE, 1.0f, TargetASC->MakeEffectContext());
		MulticastRPCSetHidden(true);
		SetActorEnableCollision(false);

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ASMHealPack::HealRespawnTimerCallback, RespawnTime);
	}
}

void ASMHealPack::HealRespawnTimerCallback()
{
	MulticastRPCSetHidden(false);
	SetActorEnableCollision(true);
}

void ASMHealPack::MulticastRPCSetHidden_Implementation(bool bNewHidden)
{
	if (!HasAuthority())
	{
		NET_LOG(this, Warning, TEXT("힐팩 숨기기: %s"), bNewHidden ? TEXT("True") : TEXT("False"));
		SetActorHiddenInGame(bNewHidden);
	}
}
