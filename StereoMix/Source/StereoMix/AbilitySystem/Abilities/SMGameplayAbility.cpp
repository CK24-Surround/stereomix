// Fill out your copyright notice in the Description page of Project Settings.


#include "SMGameplayAbility.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Characters/SMPlayerCharacter.h"

USMGameplayAbility::USMGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
}

USMAbilitySystemComponent* USMGameplayAbility::GetStereoMixAbilitySystemComponentFromActorInfo() const
{
	if (ensure(CurrentActorInfo))
	{
		return Cast<USMAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get());
	}

	return nullptr;
}

ASMPlayerCharacter* USMGameplayAbility::GetStereoMixPlayerCharacterFromActorInfo() const
{
	if (ensure(CurrentActorInfo))
	{
		return Cast<ASMPlayerCharacter>(CurrentActorInfo->AvatarActor.Get());
	}
	
	return nullptr;
}
