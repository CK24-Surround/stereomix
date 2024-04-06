// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixGameplayAbility_Shoot.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "GAS/AttributeSets/StereoMixCharacterAttributeSet.h"
#include "Utilities/StereoMixeLog.h"

UStereoMixGameplayAbility_Shoot::UStereoMixGameplayAbility_Shoot()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UStereoMixGameplayAbility_Shoot::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (ASC)
	{
		const UStereoMixCharacterAttributeSet* CharacterAttributeSet = ASC->GetSet<UStereoMixCharacterAttributeSet>();
		if (CharacterAttributeSet)
		{
			CommitAbility(Handle, ActorInfo, ActivationInfo);
			
			if (HasAuthority(&ActivationInfo))
			{
				NET_LOG(ActorInfo->AvatarActor.Get(), Log, TEXT("Shoot Activate"));
			}
		}
		else
		{
			NET_LOG(ActorInfo->AvatarActor.Get(), Warning, TEXT("호환되지 않는 GA입니다."));
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UStereoMixGameplayAbility_Shoot::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UStereoMixGameplayAbility_Shoot::OnFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
