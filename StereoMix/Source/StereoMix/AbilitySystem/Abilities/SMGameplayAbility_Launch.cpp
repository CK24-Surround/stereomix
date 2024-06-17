// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayAbility_Launch.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/SMPlayerCharacter.h"

USMGameplayAbility_Launch::USMGameplayAbility_Launch()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

	FGameplayTagContainer BlockedTags;
	BlockedTags.AddTag(SMTags::Ability::Activation::Catch);
	BlockedTags.AddTag(SMTags::Character::State::Catch);
	BlockedTags.AddTag(SMTags::Character::State::Caught);
	BlockedTags.AddTag(SMTags::Character::State::Smashing);
	BlockedTags.AddTag(SMTags::Character::State::Smashed);
	BlockedTags.AddTag(SMTags::Character::State::Stun);
	BlockedTags.AddTag(SMTags::Character::State::Immune);
	ActivationBlockedTags = BlockedTags;
}

void USMGameplayAbility_Launch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensureAlways(SourceASC))
	{
		EndAbilityByCancel();
		return;
	}

	CommitAbility(Handle, ActorInfo, ActivationInfo);

	ESMTeam SourceTeam = SourceCharacter->GetTeam();
	CachedMontage = Montage.FindOrAdd(SourceTeam, nullptr);

	// 발사 애니메이션을 재생합니다.
	SourceASC->PlayMontage(this, ActivationInfo, CachedMontage, 1.0f);
}

void USMGameplayAbility_Launch::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		const FGameplayEffectSpecHandle GESpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), 1.0f);
		if (!ensureAlways(GESpecHandle.IsValid()))
		{
			return;
		}

		FGameplayEffectSpec* GESpec = GESpecHandle.Data.Get();
		if (!ensureAlways(GESpec))
		{
			return;
		}

		const float Cooldown = 1.0f / LaunchRate;
		GESpec->SetSetByCallerMagnitude(SMTags::Data::Cooldown, Cooldown);
		(void)ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, GESpecHandle);
	}
}