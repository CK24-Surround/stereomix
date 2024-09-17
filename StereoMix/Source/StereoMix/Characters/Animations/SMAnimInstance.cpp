// Copyright Surround, Inc. All Rights Reserved.


#include "SMAnimInstance.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "KismetAnimationLibrary.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Utilities/SMLog.h"

void USMAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	SourceCharacter = Cast<ASMPlayerCharacterBase>(TryGetPawnOwner());
	if (SourceCharacter.Get())
	{
		SourceMovement = SourceCharacter->GetCharacterMovement();
	}
}

void USMAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	UpdateMovementInfo();
}

void USMAnimInstance::UpdateMovementInfo()
{
	if (SourceCharacter)
	{
		if (ensure(SourceMovement))
		{
			const FVector Acceleration = SourceMovement->GetCurrentAcceleration();
			Acceleration2D = FVector(Acceleration.X, Acceleration.Y, 0.0);
			bHasAcceleration = !Acceleration2D.IsNearlyZero();

			const FVector Velocity = SourceMovement->Velocity;
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

			// bAmICatching = SourceCharacter->bAmICatching();
			// NET_VLOG(SourceCharacter, 1, 1.0f, TEXT("LocalVelocityDirectionAngle: %f"), LocalVelocityDirectionAngle);
			// NET_VLOG(SourceCharacter, 2, 1.0f, TEXT("Velocity2D: %s"), *Velocity2D.ToString());
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
