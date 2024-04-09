// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixGameplayAbility.h"

#include "AbilitySystem/StereoMixAbilitySystemComponent.h"
#include "Characters/StereoMixPlayerCharacter.h"

UStereoMixGameplayAbility::UStereoMixGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
}

UStereoMixAbilitySystemComponent* UStereoMixGameplayAbility::GetStereoMixAbilitySystemComponentFromActorInfo() const
{
	if (ensure(CurrentActorInfo))
	{
		return Cast<UStereoMixAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get());
	}

	return nullptr;
}

AStereoMixPlayerCharacter* UStereoMixGameplayAbility::GetStereoMixPlayerCharacterFromActorInfo() const
{
	if (ensure(CurrentActorInfo))
	{
		return Cast<AStereoMixPlayerCharacter>(CurrentActorInfo->AvatarActor.Get());
	}
	
	return nullptr;
}
