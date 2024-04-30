// Copyright Surround, Inc. All Rights Reserved.


#include "SMBoostZone.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"

ASMBoostZone::ASMBoostZone()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(SceneComponent);
	BoxComponent->SetCollisionProfileName(SMCollisionProfileName::Gimmick);
}

void ASMBoostZone::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ASMBoostZone::BeginPlay()
{
	Super::BeginPlay();

	SetActorTickEnabled(false);
}

void ASMBoostZone::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	for (const auto& InZonePair : InZoneMap)
	{
		if (InZonePair.Key.Get())
		{
			PerformBoostZone(InZonePair.Key.Get());
		}
	}
}

void ASMBoostZone::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	USMAbilitySystemComponent* TargetASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor));
	if (ensure(TargetASC))
	{
		if (!IsActorTickEnabled())
		{
			SetActorTickEnabled(true);
		}

		InZoneMap.FindOrAdd(TargetASC);
	}
}

void ASMBoostZone::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	USMAbilitySystemComponent* TargetASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor));
	if (!ensureAlways(TargetASC))
	{
		return;
	}

	RemoveBoostZone(TargetASC);
	InZoneMap.Remove(TargetASC);

	if (InZoneMap.Num() <= 0)
	{
		SetActorTickEnabled(false);
	}
}

void ASMBoostZone::PerformBoostZone(USMAbilitySystemComponent* TargetASC)
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

	FBoostZoneDirectionData& TargetBoostZoneDirectionData = InZoneMap[TargetASC];
	const FVector BoostZoneDirection = FRotationMatrix(GetActorRotation()).GetUnitAxis(EAxis::X);
	const FVector TargetMoveDirection = TargetMovement->GetCurrentAcceleration().GetSafeNormal();
	if (BoostZoneDirection.Dot(TargetMoveDirection) > 0.0f)
	{
		TargetBoostZoneDirectionData.bNewIsCurrectDirection = true;
		if (TargetBoostZoneDirectionData.bOldIsCurrectDirection != TargetBoostZoneDirectionData.bNewIsCurrectDirection)
		{
			ApplyBoostZone(TargetASC);
		}

		TargetBoostZoneDirectionData.bOldIsCurrectDirection = true;
	}
	else
	{
		TargetBoostZoneDirectionData.bNewIsCurrectDirection = false;
		if (TargetBoostZoneDirectionData.bOldIsCurrectDirection != TargetBoostZoneDirectionData.bNewIsCurrectDirection)
		{
			RemoveBoostZone(TargetASC);
		}

		TargetBoostZoneDirectionData.bOldIsCurrectDirection = false;
	}
}

void ASMBoostZone::ApplyBoostZone(USMAbilitySystemComponent* TargetASC)
{
	if (ensureAlways(TargetASC && BoostZoneGE))
	{
		TargetASC->ApplyMoveSpeedMultiplyInfinite(BoostZoneGE, MoveSpeedToApplyMultiply);
	}
}

void ASMBoostZone::RemoveBoostZone(USMAbilitySystemComponent* TargetASC)
{
	if (ensureAlways(TargetASC && BoostZoneGE))
	{
		TargetASC->RemoveMoveSpeedMultiply(BoostZoneGE, MoveSpeedToApplyMultiply);
	}
}
