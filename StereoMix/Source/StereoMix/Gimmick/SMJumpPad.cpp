// Copyright Surround, Inc. All Rights Reserved.


#include "SMJumpPad.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"
#include "AbilitySystem/SMTags.h"
#include "Utilities/SMCalculateBlueprintLibrary.h"

ASMJumpPad::ASMJumpPad()
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(SceneComponent);
	BoxComponent->SetCollisionProfileName(SMCollisionProfileName::Gimmick);
	BoxComponent->InitBoxExtent(FVector(150.0, 150.0, 50.0));
	BoxComponent->SetRelativeLocation(FVector(0.0, 0.0, 50.0));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(BoxComponent);
	MeshComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	MeshComponent->SetRelativeLocationAndRotation(FVector(0.0, 0.0, -50.0), FRotator(0.0, 90.0, 0.0));

	JumpTarget = CreateDefaultSubobject<USceneComponent>(TEXT("JumpTarget"));
	JumpTarget->SetupAttachment(SceneComponent);

	DenineTags.AddTag(SMTags::Character::State::Catch);
	DenineTags.AddTag(SMTags::Character::State::Stun);
}

void ASMJumpPad::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ASMJumpPad::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	ACharacter* TargetCharacter = Cast<ACharacter>(OtherActor);
	if (ensureAlways(TargetCharacter))
	{
		if (TargetCharacter->GetLocalRole() == ROLE_SimulatedProxy)
		{
			return;
		}

		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetCharacter);
		if (!ensureAlways(TargetASC))
		{
			return;
		}

		if (TargetASC->HasAnyMatchingGameplayTags(DenineTags))
		{
			return;
		}

		Jump(TargetCharacter, JumpTarget->GetComponentLocation());
	}
}

void ASMJumpPad::Jump(ACharacter* InCharacter, const FVector& TargetLocation)
{
	const FVector StartLocation = InCharacter->GetActorLocation();
	UCharacterMovementComponent* TargetMovement = InCharacter->GetCharacterMovement();
	if (!ensureAlways(TargetMovement))
	{
		return;
	}

	// TODO: 임시로 에어컨트롤 비활성화
	TargetMovement->AirControl = 0.0f;

	TargetMovement->GravityScale = GravityScale;
	const float Gravity = TargetMovement->GetGravityZ();
	const FVector LaunchVelocity = USMCalculateBlueprintLibrary::SuggestProjectileVelocity_CustomApexHeight2(this, StartLocation, TargetLocation, ApexHeight, Gravity);

	InCharacter->LaunchCharacter(LaunchVelocity, true, true);
}
