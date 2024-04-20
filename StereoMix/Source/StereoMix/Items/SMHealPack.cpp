// Fill out your copyright notice in the Description page of Project Settings.


#include "SMHealPack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "Utilities/SMCollision.h"

ASMHealPack::ASMHealPack()
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(SceneComponent);
	SphereComponent->SetCollisionProfileName(SMCollisionProfileName::HealPack);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(SphereComponent);
	MeshComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
}

void ASMHealPack::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
	if (ensure(TargetASC))
	{
		TargetASC->BP_ApplyGameplayEffectToSelf(HealGE, 1.0f, TargetASC->MakeEffectContext());
	}
}
