// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_NoiseBreakIndicator.h"

#include "AbilitySystem/SMTags.h"
#include "AbilitySystem/Abilities/NoiseBreak/SMGA_NoiseBreak.h"
#include "AbilitySystem/Task/SMAT_SkillIndicator.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"


USMGA_NoiseBreakIndicator::USMGA_NoiseBreakIndicator()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;

	AbilityTags.AddTag(SMTags::Ability::NoiseBreakIndicator);
}

void USMGA_NoiseBreakIndicator::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	const USMPlayerCharacterDataAsset* SourceDataAsset = GetDataAsset();
	if (!SourceCharacter || !SourceDataAsset)
	{
		EndAbilityByCancel();
		return;
	}

	// 노이즈 브레이크 데이터를 통해 사거리를 가져옵니다.
	const USMGA_NoiseBreak* NoiseBreakDefaultObject = SourceDataAsset->DefaultActiveAbilities[EActiveAbility::NoiseBreak]->GetDefaultObject<USMGA_NoiseBreak>();
	if (!NoiseBreakDefaultObject)
	{
		EndAbilityByCancel();
		return;
	}

	UNiagaraComponent* NoiseBreakIndicator = SourceCharacter->GetNoiseBreakIndicator();
	const float NoiseBreakMaxDistance = NoiseBreakDefaultObject->GetMaxDistance();
	USMAT_SkillIndicator* SkillIndicatorTask = USMAT_SkillIndicator::SkillIndicator(this, NoiseBreakIndicator, NoiseBreakMaxDistance, true);
	SkillIndicatorTask->ReadyForActivation();
}
