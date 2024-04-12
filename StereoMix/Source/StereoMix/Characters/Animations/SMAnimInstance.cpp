// Fill out your copyright notice in the Description page of Project Settings.


#include "SMAnimInstance.h"

#include "KismetAnimationLibrary.h"
#include "Characters/SMPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Utilities/SMLog.h"

void USMAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	SourceCharacter = Cast<ASMPlayerCharacter>(TryGetPawnOwner());
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
		UCharacterMovementComponent* SourceMovement = SourceCharacter->GetCharacterMovement();
		if (ensure(SourceMovement))
		{
			Velocity = SourceMovement->Velocity;
			DeltaSpeed = Velocity.Size() - LastSpeed;
			Acceleration = SourceMovement->GetCurrentAcceleration();
			bHasAcceleration = Acceleration != FVector::ZeroVector;
			LocalVelocityDirectionAngle = UKismetAnimationLibrary::CalculateDirection(Velocity, SourceCharacter->GetActorRotation());
			UpdateDirection();

			LastSpeed = Velocity.Size();
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
