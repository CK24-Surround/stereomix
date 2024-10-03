// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_SkillGaugeControl.h"

#include "AbilitySystem/Task/SMAT_CheckTeamTileEntryExit.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"

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
	CheckTeamTileTask->OnTeamTileEntry.BindUObject(this, &ThisClass::OnTeamTileEntry);
	CheckTeamTileTask->OnTeamTileExit.BindUObject(this, &ThisClass::OnTeamTileExit);
	CheckTeamTileTask->ReadyForActivation();
}

void USMGA_SkillGaugeControl::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	BP_RemoveGameplayEffectFromOwnerWithHandle(SkillGaugeGEHandle);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USMGA_SkillGaugeControl::OnTeamTileEntry()
{
	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	const USMPlayerCharacterDataAsset* SourceDataAsset = GetDataAsset();
	if (!SourceCharacter || !SourceDataAsset)
	{
		return;
	}

	SkillGaugeGEHandle = K2_ApplyGameplayEffectSpecToOwner(MakeOutgoingGameplayEffectSpec(SourceDataAsset->SkillGaugeGE));
}

void USMGA_SkillGaugeControl::OnTeamTileExit()
{
	BP_RemoveGameplayEffectFromOwnerWithHandle(SkillGaugeGEHandle);
}
