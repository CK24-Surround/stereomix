// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_Attack.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"

USMGA_Attack::USMGA_Attack()
{
	ActivationBlockedTags.AddTag(SMTags::Character::State::Common::Blocking::Attack);
	ActivationBlockedTags.AddTag(SMTags::Ability::Activation::Hold);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Common::Uncontrollable);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Common::Hold);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Common::Held);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Common::NoiseBreak);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Common::NoiseBreaked);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Common::Neutralized);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Common::Immune);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Common::Stun);
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

bool USMGA_Attack::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	const USMAbilitySystemComponent* SourceASC = GetASC();
	if (SourceASC && SourceASC->HasMatchingGameplayTag(SMTags::Character::State::Common::Uncontrollable))
	{
		return false;
	}
	
	if (SourceASC && SourceASC->HasMatchingGameplayTag(SMTags::Character::State::Common::UnlimitStamina))
	{
		return true;
	}

	if (SourceASC && SourceASC->HasMatchingGameplayTag(SMTags::Character::State::Common::Exhausted))
	{
		return false;
	}

	return true;
}

void USMGA_Attack::OnCanceled(FGameplayEventData Payload)
{
	K2_EndAbility();
}
