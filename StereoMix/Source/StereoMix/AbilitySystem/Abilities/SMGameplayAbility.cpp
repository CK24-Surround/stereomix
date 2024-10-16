// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayAbility.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Components/Character/SMHIC_Character.h"
#include "FunctionLibraries/SMHoldInteractionBlueprintLibrary.h"
#include "Utilities/SMLog.h"

USMGameplayAbility::USMGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
}

USMAbilitySystemComponent* USMGameplayAbility::GetASC() const
{
	if (!ensureAlways(CurrentActorInfo))
	{
		return nullptr;
	}

	return Cast<USMAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get());
}

AActor* USMGameplayAbility::GetAvatarActor() const
{
	if (!ensureAlways(CurrentActorInfo))
	{
		return nullptr;
	}

	return CurrentActorInfo->AvatarActor.Get();
}

ASMPlayerCharacterBase* USMGameplayAbility::GetCharacter() const
{
	if (!ensureAlways(CurrentActorInfo))
	{
		return nullptr;
	}

	return Cast<ASMPlayerCharacterBase>(CurrentActorInfo->AvatarActor.Get());
}

USMHIC_Character* USMGameplayAbility::GetHIC() const
{
	if (!ensureAlways(CurrentActorInfo))
	{
		return nullptr;
	}

	return USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent<USMHIC_Character>(CurrentActorInfo->AvatarActor.Get());
}

const USMPlayerCharacterDataAsset* USMGameplayAbility::GetDataAsset() const
{
	if (!ensureAlways(CurrentActorInfo))
	{
		return nullptr;
	}

	ASMPlayerCharacterBase* SourceCharacter = Cast<ASMPlayerCharacterBase>(CurrentActorInfo->AvatarActor.Get());
	if (!SourceCharacter)
	{
		return nullptr;
	}

	return SourceCharacter->GetDataAsset();
}

void USMGameplayAbility::EndAbilityByCancel()
{
	check(CurrentActorInfo);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
