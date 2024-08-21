// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_Slash.h"

#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/SMBassCharacter.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "Utilities/SMLog.h"

USMGA_Slash::USMGA_Slash()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
}

void USMGA_Slash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	K2_CommitAbility();

	Slash();
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

void USMGA_Slash::Slash()
{
	ASMBassCharacter* SourceCharacter = Cast<ASMBassCharacter>(GetAvatarActorFromActorInfo());
	if (!ensureAlways(SourceCharacter))
	{
		K2_EndAbility();
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensureAlways(SourceASC))
	{
		EndAbilityByCancel();
		return;
	}

	ESMTeam SourceTeam = SourceCharacter->GetTeam();
	const USMPlayerCharacterDataAsset* CharacterDataAsset = SourceCharacter->GetDataAsset();
	if (CharacterDataAsset)
	{
		SourceASC->PlayMontage(this, CurrentActivationInfo, CharacterDataAsset->AttackMontage[SourceTeam], 1.0f);
	}

	if (K2_HasAuthority())
	{
		SourceCharacter->Slash(Distance, Angle, SlashTime);
		SourceCharacter->OnSlashEndSignature.BindUObject(this, &ThisClass::SlashEnded);
	}

	K2_EndAbility();
}

void USMGA_Slash::SlashEnded()
{
	ASMBassCharacter* SourceCharacter = Cast<ASMBassCharacter>(GetAvatarActorFromActorInfo());
	if (!ensureAlways(SourceCharacter))
	{
		K2_EndAbility();
		return;
	}

	NET_LOG(SourceCharacter, Warning, TEXT("Slash Ended"));
	SourceCharacter->OnSlashEndSignature.Unbind();
}
