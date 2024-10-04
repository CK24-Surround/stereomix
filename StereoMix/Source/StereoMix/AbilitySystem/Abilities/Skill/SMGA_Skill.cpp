// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_Skill.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/SMTags.h"

USMGA_Skill::USMGA_Skill()
{
	ActivationBlockedTags.AddTag(SMTags::Ability::Activation::Hold);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Common::Hold);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Common::Held);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Common::NoiseBreak);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Common::NoiseBreaked);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Common::Neutralized);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Common::Immune);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Common::Stun);
}

void USMGA_Skill::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilityTask_WaitGameplayEvent* NeutralizeWaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::Character::Neutralize);
	NeutralizeWaitTask->EventReceived.AddDynamic(this, &ThisClass::OnCanceled);
	NeutralizeWaitTask->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* StunWaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::Character::Stun);
	StunWaitTask->EventReceived.AddDynamic(this, &ThisClass::OnCanceled);
	StunWaitTask->ReadyForActivation();
}

void USMGA_Skill::OnCanceled(FGameplayEventData Payload)
{
	K2_EndAbility();
}
