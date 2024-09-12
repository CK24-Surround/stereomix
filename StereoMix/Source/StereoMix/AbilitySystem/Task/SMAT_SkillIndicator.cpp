// Copyright Surround, Inc. All Rights Reserved.


#include "SMAT_SkillIndicator.h"

#include "NiagaraComponent.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "Utilities/SMLog.h"

USMAT_SkillIndicator::USMAT_SkillIndicator()
{
	bTickingTask = true;
}

USMAT_SkillIndicator* USMAT_SkillIndicator::SkillIndicator(UGameplayAbility* OwningAbility, ASMPlayerCharacterBase* NewSourceCharacter, UNiagaraComponent* NewIndicator, float NewMaxDistance)
{
	USMAT_SkillIndicator* NewObj = NewAbilityTask<USMAT_SkillIndicator>(OwningAbility);
	NewObj->SourceCharacter = NewSourceCharacter;
	NewObj->Indicator = NewIndicator;
	NewObj->MaxDistance = NewMaxDistance;
	return NewObj;
}

void USMAT_SkillIndicator::Activate()
{
	if (!SourceCharacter.Get() || !Indicator.Get())
	{
		return;
	}

	UCapsuleComponent* CapsuleComponent = SourceCharacter->GetCapsuleComponent();
	if (CapsuleComponent)
	{
		CapsuleZOffset = CapsuleComponent->GetScaledCapsuleHalfHeight();
	}

	bHasMaxDistance = MaxDistance > 0.0f;
	Indicator->Activate(true);
}

void USMAT_SkillIndicator::TickTask(float DeltaTime)
{
	if (!SourceCharacter.Get() || !Indicator.Get())
	{
		return;
	}

	const FVector CursorLocation = SourceCharacter->GetLocationFromCursor(true);
	FVector TargetLocation = CursorLocation;
	if (bHasMaxDistance)
	{
		const FVector SourceLocation = SourceCharacter->GetActorLocation() - FVector(0.0, 0.0, CapsuleZOffset);
		if ((TargetLocation - SourceLocation).SizeSquared() >= FMath::Square(MaxDistance))
		{
			const FVector TargetDirection = (CursorLocation - SourceLocation).GetSafeNormal2D();
			TargetLocation = SourceLocation + (TargetDirection * MaxDistance);
		}
	}

	const FVector Offset(0.0, 0.0, 15.0f);
	TargetLocation += Offset;
	Indicator->SetWorldLocation(TargetLocation);
}

void USMAT_SkillIndicator::OnDestroy(bool bInOwnerFinished)
{
	if (Indicator.Get())
	{
		Indicator->DeactivateImmediate();
	}

	Super::OnDestroy(bInOwnerFinished);
}
