// Fill out your copyright notice in the Description page of Project Settings.


#include "SMJumpPad.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Utilities/SMCollision.h"

ASMJumpPad::ASMJumpPad()
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;
	
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(SceneComponent);
	BoxComponent->SetCollisionProfileName(SMCollisionProfileName::Trigger);
}

void ASMJumpPad::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	ACharacter* TargetCharacter = Cast<ACharacter>(OtherActor);
	if (ensureAlways(TargetCharacter))
	{
		Jump(TargetCharacter, FVector::ZeroVector);
	}
}

void ASMJumpPad::Jump(ACharacter* InCharacter, const FVector& TargetLocation)
{
	const FVector StartLocation = InCharacter->GetActorLocation();

	FVector LaunchVelocity;
	UGameplayStatics::SuggestProjectileVelocity(this, LaunchVelocity, StartLocation, TargetLocation, 980.0f, true, 0.0f, 0.0f, ESuggestProjVelocityTraceOption::DoNotTrace);

	FPredictProjectilePathParams Params;
	Params.StartLocation = StartLocation;
	Params.LaunchVelocity = LaunchVelocity;
	Params.bTraceWithCollision = true;
	Params.ProjectileRadius = 15.0f;
	Params.MaxSimTime = 5.0f;
	Params.OverrideGravityZ = 1.0f;
	FPredictProjectilePathResult Result;

	if (UGameplayStatics::PredictProjectilePath(this, Params, Result))
	{
		InCharacter->LaunchCharacter(LaunchVelocity, true, true);
	}
}
