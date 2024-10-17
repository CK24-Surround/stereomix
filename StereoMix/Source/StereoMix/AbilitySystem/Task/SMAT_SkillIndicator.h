// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "SMAT_SkillIndicator.generated.h"

class UNiagaraComponent;
class ASMPlayerCharacterBase;

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMAT_SkillIndicator : public UAbilityTask
{
	GENERATED_BODY()

public:
	enum class EIndicatorAlignment : uint8
	{
		CharacterCenter, // 캐릭터의 중심으로 인디케이터를 생성합니다.
		Ground,          // 캐릭터의 바닥위치를 중심으로 인디케이터를 생성합니다.
		Tile             // 캐릭터의 바닥위치를 중심으로 인디케이터를 생성하고 타일로 스냅합니다.
	};

	USMAT_SkillIndicator();

	static USMAT_SkillIndicator* SkillIndicator(UGameplayAbility* OwningAbility, UNiagaraSystem* IndicatorVFX, EIndicatorAlignment NewIndicatorAlignment, const TOptional<float>& OptionalMaxDistance = TOptional<float>());

protected:
	virtual void Activate() override;

	virtual void OnDestroy(bool bInOwnerFinished) override;

	virtual void TickTask(float DeltaTime) override;

	TWeakObjectPtr<ASMPlayerCharacterBase> OwnerCharacter;

	TObjectPtr<UNiagaraComponent> IndicatorVFXComponent;

	TOptional<float> MaxDistance = TOptional<float>();

	EIndicatorAlignment IndicatorAlignment = EIndicatorAlignment::CharacterCenter;

	const FName MaxRangeParameterName = TEXT("MaxRange");
	const FName CurrentRangeParameterName = TEXT("CurrentRange");
	const FName TeamParameterName = TEXT("Team");
};
