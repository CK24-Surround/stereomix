// Copyright Studio Surround. All Rights Reserved.


#include "SMAT_AdjustableDash.h"

#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Utilities/SMLog.h"


USMAT_AdjustableDash::USMAT_AdjustableDash()
{
	bTickingTask = true;
}

USMAT_AdjustableDash* USMAT_AdjustableDash::AdjustableDash(UGameplayAbility* OwningAbility, float RotationPerSecond)
{
	USMAT_AdjustableDash* NewObj = NewAbilityTask<USMAT_AdjustableDash>(OwningAbility);
	NewObj->SourceCharacter = Cast<ASMPlayerCharacterBase>(OwningAbility->GetAvatarActorFromActorInfo());
	NewObj->RotationPerSecond = RotationPerSecond;

	return NewObj;
}

void USMAT_AdjustableDash::TickTask(float DeltaTime)
{
	AController* SourceController = SourceCharacter.Get() ? SourceCharacter->GetController() : nullptr;
	if (!SourceController)
	{
		return;
	}

	const FVector SourceLocation = SourceCharacter->GetActorLocation();
	const FVector CursorLocation = SourceCharacter->GetLocationFromCursor();

	const FVector SourceDirection = SourceCharacter->GetActorForwardVector();
	const FVector SourceToCursorDirection = (CursorLocation - SourceLocation).GetSafeNormal();

	const FVector CrossProduct = FVector::CrossProduct(SourceDirection, SourceToCursorDirection);
	const float CrossProductZ = CrossProduct.Z;

	const float RotationPower = RotationPerSecond * DeltaTime;
	float DeltaYaw;

	const float AllowedRange = 0.05f;
	if (FMath::IsNearlyZero(CrossProductZ, AllowedRange))
	{
		DeltaYaw = 0.0f;
	}
	else if (CrossProductZ > 0)
	{
		DeltaYaw = RotationPower;
	}
	else
	{
		DeltaYaw = -RotationPower;
	}

	const FRotator SourceRotation = SourceCharacter->GetActorRotation();
	const FRotator NewRotation = SourceRotation + FRotator(0.0, DeltaYaw, 0.0);
	SourceController->SetControlRotation(NewRotation);
}
