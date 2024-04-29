// Copyright Surround, Inc. All Rights Reserved.


#include "SMBoostZone.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
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

	SetActorTickEnabled(false);

	// if (!HasAuthority())
	// {
	// 	SetActorEnableCollision(false);
	// }
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

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
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

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
	if (!ensureAlways(TargetASC))
	{
		return;
	}

	ACharacter* TargetCharacter = Cast<ACharacter>(TargetASC->GetAvatarActor());
	if (TargetCharacter->IsLocallyControlled())
	{
		TargetCharacter->GetCharacterMovement()->MaxWalkSpeed = 700.0f;
	}
	
	TargetASC->BP_ApplyGameplayEffectToSelf(RemoveBoostZoneGE, 1.0f, TargetASC->MakeEffectContext());
	InZoneMap.Remove(TargetASC);

	if (InZoneMap.Num() <= 0)
	{
		SetActorTickEnabled(false);
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

	FBoostZoneDirectionData& TargetBoostZoneDirectionData = InZoneMap[TargetASC];
	const FVector BoostZoneDirection = FRotationMatrix(GetActorRotation()).GetUnitAxis(EAxis::X);
	const FVector TargetMoveDirection = TargetMovement->GetCurrentAcceleration().GetSafeNormal();
	if (BoostZoneDirection.Dot(TargetMoveDirection) > 0.0f)
	{
		TargetBoostZoneDirectionData.bNewIsCurrectDirection = true;
		if (TargetBoostZoneDirectionData.bOldIsCurrectDirection != TargetBoostZoneDirectionData.bNewIsCurrectDirection)
		{
			if (TargetCharacter->IsLocallyControlled())
			{
				TargetMovement->MaxWalkSpeed = TargetMovement->MaxWalkSpeed * 1.5;
			}

			TargetASC->BP_ApplyGameplayEffectToSelf(ApplyBoostZoneGE, 1.0f, TargetASC->MakeEffectContext());
		}

		TargetBoostZoneDirectionData.bOldIsCurrectDirection = true;
	}
	else
	{
		TargetBoostZoneDirectionData.bNewIsCurrectDirection = false;
		if (TargetBoostZoneDirectionData.bOldIsCurrectDirection != TargetBoostZoneDirectionData.bNewIsCurrectDirection)
		{
			if (TargetCharacter->IsLocallyControlled())
			{
				TargetMovement->MaxWalkSpeed = 700.0f;
			}

			TargetASC->BP_ApplyGameplayEffectToSelf(RemoveBoostZoneGE, 1.0f, TargetASC->MakeEffectContext());
		}

		TargetBoostZoneDirectionData.bOldIsCurrectDirection = false;
	}
}
