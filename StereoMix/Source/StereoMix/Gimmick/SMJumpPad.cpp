// Fill out your copyright notice in the Description page of Project Settings.


#include "SMJumpPad.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"

ASMJumpPad::ASMJumpPad()
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(SceneComponent);
	BoxComponent->SetCollisionProfileName(SMCollisionProfileName::Trigger);
	BoxComponent->InitBoxExtent(FVector(75.0, 75.0, 50.0));
	BoxComponent->SetRelativeLocation(FVector(75.0, 75.0, 50.0));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(BoxComponent);
	MeshComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	MeshComponent->SetRelativeLocation(FVector(0.0, 0.0, -50.0));
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
		if (TargetCharacter->HasAuthority() || TargetCharacter->IsLocallyControlled())
		{
			Jump(TargetCharacter, LandingLocation);
		}
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

	TargetMovement->GravityScale = GravityScale;
	// TODO: 임시로 에어컨트롤 비활성화
	TargetMovement->AirControl = 0.0f;

	FVector LaunchVelocity;
	const float Gravity = GetWorld()->GetGravityZ() * TargetMovement->GravityScale;
	UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, LaunchVelocity, StartLocation, TargetLocation, Gravity, ArcRatio);

	InCharacter->LaunchCharacter(LaunchVelocity, true, true);
}
