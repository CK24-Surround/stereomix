// Copyright Surround, Inc. All Rights Reserved.


#include "SMBoostZone.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AttributeSets/SMCharacterAttributeSet.h"
#include "Characters/SMPlayerCharacter.h"
#include "Components/BoxComponent.h"
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

	ASMPlayerCharacter* TargetCharacter = Cast<ASMPlayerCharacter>(OtherActor);
	if (ensure(TargetCharacter))
	{
		if (!IsActorTickEnabled())
		{
			SetActorTickEnabled(true);
		}

		InZoneMap.FindOrAdd(TargetCharacter);
	}
}

void ASMBoostZone::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	ASMPlayerCharacter* TargetCharacter = Cast<ASMPlayerCharacter>(OtherActor);
	if (!ensureAlways(TargetCharacter))
	{
		return;
	}

	if (InZoneMap[TargetCharacter].bNewIsCurrectDirection)
	{
		RemoveBoostZone(TargetCharacter);
	}

	InZoneMap.Remove(TargetCharacter);

	if (InZoneMap.Num() <= 0)
	{
		SetActorTickEnabled(false);
	}
}

void ASMBoostZone::PerformBoostZone(ASMPlayerCharacter* TargetCharacter)
{
	if (!ensure(TargetCharacter))
	{
		return;
	}

	UCharacterMovementComponent* TargetMovement = TargetCharacter->GetCharacterMovement();
	if (!ensure(TargetMovement))
	{
		return;
	}

	FBoostZoneDirectionData& TargetBoostZoneDirectionData = InZoneMap[TargetCharacter];
	const FVector BoostZoneDirection = FRotationMatrix(GetActorRotation()).GetUnitAxis(EAxis::X);
	const FVector TargetMoveDirection = TargetMovement->GetCurrentAcceleration().GetSafeNormal();
	if (BoostZoneDirection.Dot(TargetMoveDirection) > 0.0f)
	{
		TargetBoostZoneDirectionData.bNewIsCurrectDirection = true;
		if (TargetBoostZoneDirectionData.bOldIsCurrectDirection != TargetBoostZoneDirectionData.bNewIsCurrectDirection)
		{
			ApplyBoostZone(TargetCharacter);
		}

		TargetBoostZoneDirectionData.bOldIsCurrectDirection = true;
	}
	else
	{
		TargetBoostZoneDirectionData.bNewIsCurrectDirection = false;
		if (TargetBoostZoneDirectionData.bOldIsCurrectDirection != TargetBoostZoneDirectionData.bNewIsCurrectDirection)
		{
			RemoveBoostZone(TargetCharacter);
		}

		TargetBoostZoneDirectionData.bOldIsCurrectDirection = false;
	}
}

void ASMBoostZone::ApplyBoostZone(ASMPlayerCharacter* TargetCharacter)
{
	if (!ensureAlways(TargetCharacter))
	{
		return;
	}

	UAbilitySystemComponent* TargetASC = TargetCharacter->GetAbilitySystemComponent();
	if (!ensureAlways(TargetASC))
	{
		return;
	}

	const USMCharacterAttributeSet* TargetAttributeSet = TargetASC->GetSet<USMCharacterAttributeSet>();
	if (!ensureAlways(TargetAttributeSet))
	{
		return;
	}

	const float MoveSpeedToAdd = (TargetAttributeSet->GetBaseMoveSpeed() * MoveSpeedToApplyMultiply) - TargetAttributeSet->GetBaseMoveSpeed();
	AddMoveSpeed(TargetCharacter, MoveSpeedToAdd);
}

void ASMBoostZone::RemoveBoostZone(ASMPlayerCharacter* TargetCharacter)
{
	if (!ensureAlways(TargetCharacter))
	{
		return;
	}

	UAbilitySystemComponent* TargetASC = TargetCharacter->GetAbilitySystemComponent();
	if (!ensureAlways(TargetASC))
	{
		return;
	}

	const USMCharacterAttributeSet* TargetAttributeSet = TargetASC->GetSet<USMCharacterAttributeSet>();
	if (!ensureAlways(TargetAttributeSet))
	{
		return;
	}

	const float MoveSpeedToAdd = (TargetAttributeSet->GetBaseMoveSpeed() * MoveSpeedToApplyMultiply) - TargetAttributeSet->GetBaseMoveSpeed();
	AddMoveSpeed(TargetCharacter, -MoveSpeedToAdd);
}

void ASMBoostZone::AddMoveSpeed(ASMPlayerCharacter* TargetCharacter, float MoveSpeedToAdd)
{
	if (!ensureAlways(TargetCharacter))
	{
		return;
	}

	UCharacterMovementComponent* TargetMovement = TargetCharacter->GetCharacterMovement();
	if (!ensureAlways(TargetMovement))
	{
		return;
	}

	if (TargetCharacter->IsLocallyControlled())
	{
		TargetMovement->MaxWalkSpeed += MoveSpeedToAdd;
	}

	if (TargetCharacter->HasAuthority())
	{
		TargetCharacter->SetMaxWalkSpeed(TargetMovement->MaxWalkSpeed + MoveSpeedToAdd);
	}
}
