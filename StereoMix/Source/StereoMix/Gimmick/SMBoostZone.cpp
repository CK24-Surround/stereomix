// Fill out your copyright notice in the Description page of Project Settings.


#include "SMBoostZone.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Components/BoxComponent.h"
#include "Utilities/SMCollision.h"

ASMBoostZone::ASMBoostZone()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickInterval(0.1f);

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(SceneComponent);
	BoxComponent->SetCollisionProfileName(SMCollisionProfileName::Trigger);
}

void ASMBoostZone::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SetActorTickEnabled(false);

	if (!HasAuthority())
	{
		SetActorEnableCollision(false);
	}
}

void ASMBoostZone::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// TODO: PlayerASCsInZone를 순회하며 방향이 맞는 경우 이동속도를 증가시키는 GE를 적용, 반대인 경우 GE를 제거합니다.
}

void ASMBoostZone::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (!IsActorTickEnabled())
	{
		SetActorTickEnabled(true);
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
	if (ensure(TargetASC))
	{
		PlayerASCsInZone.AddUnique(TargetASC);
	}
}

void ASMBoostZone::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	// TODO: PlayerASCsInZone에서 해당 ASC를 제거하고 PlayerASCsInZone의 요소가 0이되면 틱을 종료시킵니다.
}
