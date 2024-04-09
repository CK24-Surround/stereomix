// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixGameplayAbility.h"

#include "AbilitySystem/StereoMixAbilitySystemComponent.h"
#include "Characters/StereoMixCharacter.h"

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


AStereoMixCharacter* UStereoMixGameplayAbility::GetStereoMixCharacterFromActorInfo() const
{
	if (ensure(CurrentActorInfo))
	{
		return Cast<AStereoMixCharacter>(CurrentActorInfo->AvatarActor.Get());
	}
	
	return nullptr;
}
