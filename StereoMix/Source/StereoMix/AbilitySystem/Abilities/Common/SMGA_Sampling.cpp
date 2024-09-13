// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_Sampling.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "AbilitySystem/Task/SMAT_CheckUnderTile.h"
#include "Characters/Player/SMPlayerCharacterBase.h"

USMGA_Sampling::USMGA_Sampling()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

	DeactivateTags.AddTag(SMTags::Character::State::Neutralize);
	DeactivateTags.AddTag(SMTags::Character::State::Immune);
	DeactivateTags.AddTag(SMTags::Character::State::NoiseBreak);

	ActivationBlockedTags.AddTag(SMTags::Character::State::Stun);
}

void USMGA_Sampling::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	USMAT_CheckUnderTile* CheckUnderTileTask = USMAT_CheckUnderTile::CheckUnderTile(this, SourceCharacter);
	CheckUnderTileTask->OnUnderTileChanged.BindUObject(this, &ThisClass::OnUnderTileChanged);
	CheckUnderTileTask->ReadyForActivation();
}

void USMGA_Sampling::OnUnderTileChanged(ASMTile* UnderTile)
{
	if (!UnderTile)
	{
		return;
	}

	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!SourceCharacter)
	{
		return;
	}

	UAbilitySystemComponent* SourceASC = GetASC();
	if (!SourceASC)
	{
		return;
	}

	if (SourceASC->HasAnyMatchingGameplayTags(DeactivateTags))
	{
		return;
	}

	const ESMTeam SourceTeam = SourceCharacter->GetTeam();
	const ESMTeam TileTeam = UnderTile->GetTeam();

	if (SourceTeam == TileTeam)
	{
		return;
	}

	UnderTile->TileTrigger(SourceTeam);
}
