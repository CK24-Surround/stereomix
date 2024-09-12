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
	if (!ensureAlways(CapsuleComponent))
	{
		return;
	}

	const float CapsuleZOffset = CapsuleComponent->GetScaledCapsuleHalfHeight();
	CapsuleOffset = FVector(0.0, 0.0, -CapsuleZOffset);

	bHasMaxDistance = MaxDistance > 0.0f;
	Indicator->Activate(true);
}

void USMAT_SkillIndicator::TickTask(float DeltaTime)
{
	if (!SourceCharacter.Get() || !Indicator.Get())
	{
		return;
	}

	const float NewMaxDistance = bHasMaxDistance ? MaxDistance : -1.0f;
	FVector TargetLocation = TargetLocation = SourceCharacter->GetLocationFromCursor(true, NewMaxDistance);

	// TODO: 이펙트가 잘 보이지 않아 넣은 오프셋입니다. 나중에 이펙트 수정 시 제거해야합니다.
	const FVector NiagaraOffset(0.0, 0.0, 15.0f);
	TargetLocation += NiagaraOffset;
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
