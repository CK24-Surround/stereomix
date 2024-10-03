// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_StaminaControl.h"

#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"

USMGA_StaminaControl::USMGA_StaminaControl()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void USMGA_StaminaControl::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	const USMPlayerCharacterDataAsset* SourceDataAsset = SourceCharacter->GetDataAsset();
	if (!SourceDataAsset)
	{
		EndAbilityByCancel();
		return;
	}

	StaminaGEHandle = K2_ApplyGameplayEffectSpecToOwner(MakeOutgoingGameplayEffectSpec(SourceDataAsset->StaminaGE));
}

void USMGA_StaminaControl::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	BP_RemoveGameplayEffectFromOwnerWithHandle(StaminaGEHandle);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
