// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_Skill.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/SMTags.h"
#include "GameInstance/SMGameInstance.h"

USMGA_Skill::USMGA_Skill()
{
	ActivationBlockedTags.AddTag(SMTags::Character::State::Hold);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Holded);
	ActivationBlockedTags.AddTag(SMTags::Character::State::NoiseBreak);
	ActivationBlockedTags.AddTag(SMTags::Character::State::NoiseBreaked);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Neutralize);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Immune);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Stun);
}

FSMCharacterSkillData* USMGA_Skill::GetSkillData(ESMCharacterType CharacterType)
{
	UWorld* World = Super::GetWorld();
	USMGameInstance* GameInstance = World ? World->GetGameInstance<USMGameInstance>() : nullptr;
	if (!GameInstance)
	{
		return nullptr;
	}

	return GameInstance->GetCharacterSkillData(CharacterType);
}

void USMGA_Skill::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilityTask_WaitGameplayEvent* NeutralizeWaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::Character::Neutralize);
	NeutralizeWaitTask->EventReceived.AddDynamic(this, &ThisClass::OnNeutralized);
	NeutralizeWaitTask->ReadyForActivation();
}

void USMGA_Skill::OnNeutralized(FGameplayEventData Payload)
{
	K2_EndAbility();
}
