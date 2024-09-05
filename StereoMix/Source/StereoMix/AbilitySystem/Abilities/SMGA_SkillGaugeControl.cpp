// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_SkillGaugeControl.h"

#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "Task/SMAT_CheckTeamTileEntryExit.h"

USMGA_SkillGaugeControl::USMGA_SkillGaugeControl()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void USMGA_SkillGaugeControl::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	USMAT_CheckTeamTileEntryExit* CheckTeamTileTask = USMAT_CheckTeamTileEntryExit::CheckTeamTileEntryExit(this, SourceCharacter);
	if (!ensureAlways(CheckTeamTileTask))
	{
		EndAbilityByCancel();
		return;
	}
	CheckTeamTileTask->OnTeamTileEntry.BindUObject(this, &ThisClass::OnTeamTileEntry);
	CheckTeamTileTask->OnTeamTileExit.BindUObject(this, &ThisClass::OnTeamTileExit);
	CheckTeamTileTask->ReadyForActivation();
}

void USMGA_SkillGaugeControl::OnTeamTileEntry()
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!SourceCharacter)
	{
		return;
	}

	const USMPlayerCharacterDataAsset* SourceDataAsset = SourceCharacter->GetDataAsset();
	if (!SourceDataAsset)
	{
		return;
	}

	SkillGaugeGEHandle = K2_ApplyGameplayEffectSpecToOwner(MakeOutgoingGameplayEffectSpec(SourceDataAsset->SkillGaugeGE));
}

void USMGA_SkillGaugeControl::OnTeamTileExit()
{
	BP_RemoveGameplayEffectFromOwnerWithHandle(SkillGaugeGEHandle);
}
