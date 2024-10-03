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
	SourceMovement = SourceCharacter ? SourceCharacter->GetCharacterMovement() : nullptr;
}

void USMAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (SourceCharacter && SourceMovement)
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

		DisplacementSpeed = DeltaSeconds > UE_KINDA_SMALL_NUMBER ? DisplacementSinceLastUpdate / DeltaSeconds : 0.0f;

		LocalVelocityDirectionAngle = UKismetAnimationLibrary::CalculateDirection(Velocity2D, SourceCharacter->GetActorRotation());
		Direction = VelocityDirectionAngleToDirection(LocalVelocityDirectionAngle);

		const FRotator CurrentRotation = SourceCharacter->GetActorRotation();
		float DeltaYaw = CurrentRotation.Yaw - PreviousYaw;
		DeltaYaw = FMath::IsNearlyZero(DeltaYaw, 0.01f) ? 0.0f : DeltaYaw;
		PreviousYaw = CurrentRotation.Yaw;
		DeltaYawDirection = DeltaYawToDirection(DeltaYaw);
		DeltaYawSpeed = FMath::Abs(DeltaYaw);
	}
}

EDirection USMAnimInstance::VelocityDirectionAngleToDirection(float InLocalVelocityDirectionAngle)
{
	if (FMath::Abs(InLocalVelocityDirectionAngle) <= 60)
	{
		return EDirection::Forward;
	}
	else if (FMath::Abs(InLocalVelocityDirectionAngle) >= 120)
	{
		return EDirection::Backward;
	}
	else if (InLocalVelocityDirectionAngle >= 0)
	{
		return EDirection::Right;
	}
	else
	{
		return EDirection::Left;
	}
}

EDirection USMAnimInstance::DeltaYawToDirection(float DeltaYaw)
{
	if (DeltaYaw < 0)
	{
		return EDirection::Left;
	}
	else if (DeltaYaw > 0)
	{
		return EDirection::Right;
	}
	else
	{
		return EDirection::Forward;
	}
}
