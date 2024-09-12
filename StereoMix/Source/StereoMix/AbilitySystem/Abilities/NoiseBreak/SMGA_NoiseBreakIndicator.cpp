// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_NoiseBreakIndicator.h"

#include "SMGA_NoiseBreak.h"
#include "AbilitySystem/Task/SMAT_SkillIndicator.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"


USMGA_NoiseBreakIndicator::USMGA_NoiseBreakIndicator()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;

	// FAbilityTriggerData TriggerData;
	// TriggerData.TriggerTag = SMTags::Event::Character::NoiseBreakIndicator;
	// TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	// AbilityTriggers.Add(TriggerData);
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

	const USMGA_NoiseBreak* NoiseBreakDefaultObject = SourceDataAsset->DefaultActiveAbilities[EActiveAbility::NoiseBreak]->GetDefaultObject<USMGA_NoiseBreak>();
	if (!NoiseBreakDefaultObject)
	{
		EndAbilityByCancel();
		return;
	}

	SkillIndicatorTask = USMAT_SkillIndicator::SkillIndicator(this, SourceCharacter->GetNoiseBreakIndicator(), NoiseBreakDefaultObject->GetMaxDistance(), true);
	SkillIndicatorTask->ReadyForActivation();
}
