// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_Attack.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/SMTags.h"

USMGA_Attack::USMGA_Attack()
{
	ActivationBlockedTags.AddTag(SMTags::Ability::Activation::Hold);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Hold);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Holded);
	ActivationBlockedTags.AddTag(SMTags::Character::State::NoiseBreak);
	ActivationBlockedTags.AddTag(SMTags::Character::State::NoiseBreaked);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Neutralize);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Immune);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Stun);
}

void USMGA_Attack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (bUseOneHitKill)
	{
		Damage = 999999.0f;
	}

	UAbilityTask_WaitGameplayEvent* NeutralizeWaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::Character::Neutralize);
	NeutralizeWaitTask->EventReceived.AddDynamic(this, &ThisClass::OnCanceled);
	NeutralizeWaitTask->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* StunWaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::Character::Stun);
	StunWaitTask->EventReceived.AddDynamic(this, &ThisClass::OnCanceled);
	StunWaitTask->ReadyForActivation();
}

void USMGA_Attack::OnCanceled(FGameplayEventData Payload)
{
	K2_EndAbility();
}
