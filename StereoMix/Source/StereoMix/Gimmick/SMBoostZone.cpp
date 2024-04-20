// Fill out your copyright notice in the Description page of Project Settings.


#include "SMBoostZone.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
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

	for (const auto& PlayerASCInZone : PlayerASCsInZone)
	{
		if (PlayerASCInZone.Get())
		{
			PerformBoostZone(PlayerASCInZone.Get());
		}
	}
}

void ASMBoostZone::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
	if (ensure(TargetASC))
	{
		if (!IsActorTickEnabled())
		{
			SetActorTickEnabled(true);
		}

		PlayerASCsInZone.AddUnique(TargetASC);
	}
}

void ASMBoostZone::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
	if (ensure(TargetASC))
	{
		TargetASC->BP_ApplyGameplayEffectToSelf(RemoveBoostZoneGE, 1.0f, TargetASC->MakeEffectContext());
		PlayerASCsInZone.Remove(TargetASC);
		if (PlayerASCsInZone.Num() <= 0)
		{
			SetActorTickEnabled(false);
		}
	}
}

void ASMBoostZone::PerformBoostZone(UAbilitySystemComponent* TargetASC)
{
	// TODO: PlayerASCsInZone를 순회하며 방향이 맞는 경우 이동속도를 증가시키는 GE를 적용, 반대인 경우 GE를 제거합니다.
	ACharacter* TargetCharacter = Cast<ACharacter>(TargetASC->GetAvatarActor());
	if (!ensure(TargetCharacter))
	{
		return;
	}

	UCharacterMovementComponent* TargetMovement = TargetCharacter->GetCharacterMovement();
	if (!ensure(TargetMovement))
	{
		return;
	}

	const FVector BoostZoneDirection = FRotationMatrix(GetActorRotation()).GetUnitAxis(EAxis::X);
	const FVector TargetMoveDirection = TargetMovement->GetCurrentAcceleration().GetSafeNormal();
	if (BoostZoneDirection.Dot(TargetMoveDirection) > 0.0f)
	{
		TargetASC->BP_ApplyGameplayEffectToSelf(AddBoostZoneGE, 1.0f, TargetASC->MakeEffectContext());
	}
	else
	{
		TargetASC->BP_ApplyGameplayEffectToSelf(RemoveBoostZoneGE, 1.0f, TargetASC->MakeEffectContext());
	}
}
