// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_Slash.h"

#include "AbilitySystem/SMTags.h"
#include "Characters/SMPlayerCharacterBase.h"
#include "Utilities/SMLog.h"

void USMGA_Slash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacterBase* SourceCharacter = Cast<ASMPlayerCharacterBase>(GetAvatarActorFromActorInfo());
	if (!ensureAlways(SourceCharacter))
	{
		K2_EndAbility();
		return;
	}

	K2_CommitAbility();
	NET_VLOG(SourceCharacter, -1, 3.0f, TEXT("베기"));

	K2_EndAbility();
}

void USMGA_Slash::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	const UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		const FGameplayEffectSpecHandle GESpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass());
		if (ensureAlways(GESpecHandle.IsValid()))
		{
			FGameplayEffectSpec* GESpec = GESpecHandle.Data.Get();
			GESpec->SetSetByCallerMagnitude(SMTags::Data::Cooldown, Cooldown);
			(void)ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, GESpecHandle);
		}
	}
}
