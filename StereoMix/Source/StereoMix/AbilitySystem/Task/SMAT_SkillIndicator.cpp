// Copyright Surround, Inc. All Rights Reserved.


#include "SMAT_SkillIndicator.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "Utilities/SMLog.h"

USMAT_SkillIndicator::USMAT_SkillIndicator()
{
	bTickingTask = true;
}

USMAT_SkillIndicator* USMAT_SkillIndicator::SkillIndicator(UGameplayAbility* OwningAbility, UNiagaraSystem* IndicatorVFX, EIndicatorAlignment NewIndicatorAlignment, const TOptional<float>& OptionalMaxDistance)
{
	USMAT_SkillIndicator* NewObj = NewAbilityTask<USMAT_SkillIndicator>(OwningAbility);

	if (AActor* OwnerActor = OwningAbility->GetAvatarActorFromActorInfo())
	{
		NewObj->OwnerCharacter = Cast<ASMPlayerCharacterBase>(OwnerActor);
		NewObj->IndicatorVFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(IndicatorVFX, OwnerActor->GetRootComponent(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, false, ENCPoolMethod::ManualRelease);
	}

	NewObj->IndicatorAlignment = NewIndicatorAlignment;
	NewObj->MaxDistance = OptionalMaxDistance;

	return NewObj;
}

void USMAT_SkillIndicator::Activate()
{
	if (!OwnerCharacter.IsValid() || !IndicatorVFXComponent)
	{
		EndTask();
		return;
	}

	IndicatorVFXComponent->SetAbsolute(false, true, false);

	if (IndicatorAlignment != EIndicatorAlignment::CharacterCenter) // 캐릭터 중심이 아닌경우 무조건 바닥을 기준으로 인디케이터가 생성되야합니다.
	{
		const UCapsuleComponent* OwnerCapsule = OwnerCharacter->GetCapsuleComponent();
		const float CapsuleZOffset = OwnerCapsule ? OwnerCapsule->GetScaledCapsuleHalfHeight() : 0.0f;
		const FVector Offset(0.0, 0.0, -CapsuleZOffset);
		IndicatorVFXComponent->SetRelativeLocation(Offset);
	}

	if (MaxDistance.IsSet())
	{
		const FName MaxDistanceParameterName = TEXT("MaxRange");
		const float MaxDistanceParameterValue = MaxDistance.GetValue();
		IndicatorVFXComponent->SetFloatParameter(MaxDistanceParameterName, MaxDistanceParameterValue);
	}

	const ESMTeam OwnerTeam = OwnerCharacter->GetTeam();
	if (OwnerTeam != ESMTeam::None)
	{
		int32 TeamParameterValue;
		if (OwnerTeam == ESMTeam::EDM)
		{
			TeamParameterValue = 0;
		}
		else
		{
			TeamParameterValue = 1;
		}

		IndicatorVFXComponent->SetIntParameter(TeamParameterName, TeamParameterValue);
	}

	IndicatorVFXComponent->Activate(true);
}

void USMAT_SkillIndicator::OnDestroy(bool bInOwnerFinished)
{
	if (IndicatorVFXComponent.Get())
	{
		IndicatorVFXComponent->DeactivateImmediate();
		IndicatorVFXComponent->ReleaseToPool();
		IndicatorVFXComponent = nullptr;
	}

	Super::OnDestroy(bInOwnerFinished);
}

void USMAT_SkillIndicator::TickTask(float DeltaTime)
{
	if (!OwnerCharacter.Get() || !IndicatorVFXComponent)
	{
		EndTask();
		return;
	}

	FVector TargetLocation;
	if (IndicatorAlignment == EIndicatorAlignment::Tile)
	{
		if (!IndicatorVFXComponent->IsActive())
		{
			IndicatorVFXComponent->Activate(true);
		}

		if (!OwnerCharacter->GetTileLocationFromCursor(TargetLocation, MaxDistance.Get(-1.0f))) // TODO: 현재는 -1이 사거리 제한을 두지 않고 처리하는 상태인데, 이와 관련된 함수를 모두 Optional로 변경할 것.
		{
			IndicatorVFXComponent->DeactivateImmediate();
		}
	}
	else
	{
		const bool bUseGround = IndicatorAlignment == EIndicatorAlignment::Ground;
		TargetLocation = OwnerCharacter->GetLocationFromCursor(bUseGround, MaxDistance.Get(-1.0f)); // TODO: bUseGround로 함수의 매개변수명도 바꿀 것.
	}

	const FVector OwnerLocation = OwnerCharacter->GetActorLocation();
	const FVector OwnerToTarget = TargetLocation - OwnerLocation;
	const float OwnerToTargetDistance = OwnerToTarget.Size2D();

	if (MaxDistance.IsSet())
	{
		const float MaxRangeParameterValue = MaxDistance.GetValue() > OwnerToTargetDistance ? MaxDistance.GetValue() : OwnerToTargetDistance;
		IndicatorVFXComponent->SetFloatParameter(MaxRangeParameterName, MaxRangeParameterValue);
	}

	IndicatorVFXComponent->SetFloatParameter(CurrentRangeParameterName, OwnerToTargetDistance);

	IndicatorVFXComponent->SetRelativeRotation(OwnerToTarget.GetSafeNormal2D().ToOrientationRotator());
}
