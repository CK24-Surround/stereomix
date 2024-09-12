// Copyright Surround, Inc. All Rights Reserved.


#include "SMBassCharacter.h"

#include "AbilitySystem/SMTags.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Utilities/SMCollision.h"


ASMBassCharacter::ASMBassCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SlashColliderRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SlashColliderRootComponent"));
	SlashColliderRootComponent->SetupAttachment(RootComponent);

	SlashColliderComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("SlashColliderComponent"));
	SlashColliderComponent->SetupAttachment(SlashColliderRootComponent);
	SlashColliderComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	const double HalfDistance = 250.0;
	SlashColliderComponent->SetRelativeLocationAndRotation(FVector(HalfDistance, 0.0, 0.0), FRotator(-90.0, 0.0, 0.0));
	SlashColliderComponent->InitCapsuleSize(50.0f, HalfDistance);

	ChargeColliderComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("ChargeColliderComponent"));
	ChargeColliderComponent->SetupAttachment(RootComponent);
	ChargeColliderComponent->SetCollisionProfileName(SMCollisionProfileName::Charge);
	ChargeColliderComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	LockAimTags.AddTag(SMTags::Character::State::Charge);
	LockAimTags.AddTag(SMTags::Character::State::SlashActivation);

	LockMovementTags.AddTag(SMTags::Character::State::Charge);

	PushBackImmuneTags.AddTag(SMTags::Character::State::Charge);
}
