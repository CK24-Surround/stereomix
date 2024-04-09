// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixGameplayAbility_Smash.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Characters/StereoMixPlayerCharacter.h"
#include "Utilities/StereoMixeLog.h"

UStereoMixGameplayAbility_Smash::UStereoMixGameplayAbility_Smash()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
}

void UStereoMixGameplayAbility_Smash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!ensure(ActorInfo))
	{
		return;
	}

	const AStereoMixPlayerCharacter* StereoMixCharacter = Cast<AStereoMixPlayerCharacter>(ActorInfo->AvatarActor.Get());
	if (!StereoMixCharacter)
	{
		NET_LOG(ActorInfo->AvatarActor.Get(), Warning, TEXT("StereoMixCharacter로 캐스팅 실패했습니다."));
		return;
	}

	UAbilityTask_PlayMontageAndWait* AbilityTaskPlayMontageAndWait = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("SmashMontage"), SmashMontage);
	if (AbilityTaskPlayMontageAndWait)
	{
		// 아마 스매시는 다른 상호작용을 무시하기 때문에 OnCompleted로 충분할 것입니다.
		AbilityTaskPlayMontageAndWait->OnCompleted.AddDynamic(this, &UStereoMixGameplayAbility_Smash::OnCompleted);
		AbilityTaskPlayMontageAndWait->ReadyForActivation();
	}
}

void UStereoMixGameplayAbility_Smash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UStereoMixGameplayAbility_Smash::OnCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
