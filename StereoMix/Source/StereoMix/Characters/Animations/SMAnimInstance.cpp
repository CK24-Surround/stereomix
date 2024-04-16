// Fill out your copyright notice in the Description page of Project Settings.


#include "SMAnimInstance.h"

#include "AnimCharacterMovementLibrary.h"
#include "KismetAnimationLibrary.h"
#include "Characters/SMPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void USMAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	SourceCharacter = Cast<ASMPlayerCharacter>(TryGetPawnOwner());
	if (SourceCharacter.Get())
	{
		SourceMovement = SourceCharacter->GetCharacterMovement();
	}
}

void USMAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	UpdateMovementInfo();
}

void USMAnimInstance::UpdateMovementInfo()
{
	if (SourceCharacter.Get())
	{
		if (ensure(SourceMovement.Get()))
		{
			Acceleration = SourceMovement->GetCurrentAcceleration();
			Acceleration2D = FVector(Acceleration.X, Acceleration.Y, 0.0);
			bHasAcceleration = !Acceleration2D.IsNearlyZero();

			Velocity = SourceMovement->Velocity;
			Velocity2D = FVector(Velocity.X, Velocity.Y, 0.0);
			bHasVeloicity = !Velocity2D.IsNearlyZero();

			const FVector CurrentLocation = SourceCharacter->GetActorLocation();
			DisplacementSinceLastUpdate = (CurrentLocation - PreviousLocation).Size();
			PreviousLocation = CurrentLocation;

			const float DeltaSeconds = GetDeltaSeconds();
			if (DeltaSeconds >= UE_KINDA_SMALL_NUMBER)
			{
				DisplacementSpeed = DisplacementSinceLastUpdate / DeltaSeconds;
			}
			else
			{
				DisplacementSpeed = 0.0f;
			}

			LocalVelocityDirectionAngle = UKismetAnimationLibrary::CalculateDirection(Velocity2D, SourceCharacter->GetActorRotation());
			UpdateDirection();
		}
	}
}

void USMAnimInstance::UpdateDirection()
{
	if (FMath::Abs(LocalVelocityDirectionAngle) <= 60)
	{
		Direction = EDirection::Forward;
	}
	else if (FMath::Abs(LocalVelocityDirectionAngle) >= 120)
	{
		Direction = EDirection::Backward;
	}
	else if (LocalVelocityDirectionAngle >= 0)
	{
		Direction = EDirection::Right;
	}
	else
	{
		Direction = EDirection::Left;
	}
}

float USMAnimInstance::K2_GetDistanceToTarget() const
{
	if (!ensure(SourceMovement.Get()))
	{
		return 0.0f;
	}
	
	const FVector PredictedStopLocation = UAnimCharacterMovementLibrary::PredictGroundMovementStopLocation(Velocity2D, SourceMovement->bUseSeparateBrakingFriction, SourceMovement->BrakingFriction, SourceMovement->GroundFriction, SourceMovement->BrakingFrictionFactor, SourceMovement->BrakingDecelerationWalking);

	const float DistanceToTarget = PredictedStopLocation.Size();
	
	return DistanceToTarget;
}
