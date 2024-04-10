// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixGameplayAbility_Stun.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "..\..\Utilities\StereoMixLog.h"
#include "Utilities/StereoMixTag.h"

UStereoMixGameplayAbility_Stun::UStereoMixGameplayAbility_Stun()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

void UStereoMixGameplayAbility_Stun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!ensure(ActorInfo))
	{
		return;
	}

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (!ensure(ASC))
	{
		return;
	}

	CommitAbility(Handle, ActorInfo, ActivationInfo);

	ASC->PlayMontage(this, ActivationInfo, StunMontage, 1.0f);

	UAbilityTask_WaitDelay* WaitDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, StunTime);
	if (WaitDelayTask)
	{
		WaitDelayTask->OnFinish.AddDynamic(this, &UStereoMixGameplayAbility_Stun::OnStandUp);
		WaitDelayTask->ReadyForActivation();
	}
}

void UStereoMixGameplayAbility_Stun::OnStandUp()
{
	UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();
	if (!ensure(ASC))
	{
		return;
	}

	// 스매시 당하는 중인 경우
	if (ASC->HasMatchingGameplayTag(StereoMixTag::Character::State::Smashed))
	{
		// TODO: 스매시 당하는 중에는 기절에서 풀려나지 않습니다.
		NET_LOG(CurrentActorInfo->AvatarActor.Get(), Warning, TEXT("스턴 풀기 리젝트"));
		UAbilityTask_WaitDelay* WaitDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, 0.1f);
		if (WaitDelayTask)
		{
			WaitDelayTask->OnFinish.AddDynamic(this, &UStereoMixGameplayAbility_Stun::OnStandUp);
			WaitDelayTask->ReadyForActivation();
		}
	}
	// 잡힌 상태인 경우
	else if (ASC->HasMatchingGameplayTag(StereoMixTag::Character::State::Caught))
	{
		ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(StereoMixTag::Ability::CaughtRecover));
		
		UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("StandUp"), StandUpMontage, 1.0f);
		if (PlayMontageAndWaitTask)
		{
			PlayMontageAndWaitTask->OnCompleted.AddDynamic(this, &UStereoMixGameplayAbility_Stun::OnComplete);
			PlayMontageAndWaitTask->OnInterrupted.AddDynamic(this, &UStereoMixGameplayAbility_Stun::OnInterrupted);
			PlayMontageAndWaitTask->OnCancelled.AddDynamic(this, &UStereoMixGameplayAbility_Stun::OnCancelled);
			PlayMontageAndWaitTask->ReadyForActivation();
		}
	}
	// 기절 상태인 경우
	else
	{
		UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("StandUp"), StandUpMontage, 1.0f);
		if (PlayMontageAndWaitTask)
		{
			PlayMontageAndWaitTask->OnCompleted.AddDynamic(this, &UStereoMixGameplayAbility_Stun::OnComplete);
			PlayMontageAndWaitTask->OnInterrupted.AddDynamic(this, &UStereoMixGameplayAbility_Stun::OnInterrupted);
			PlayMontageAndWaitTask->OnCancelled.AddDynamic(this, &UStereoMixGameplayAbility_Stun::OnCancelled);
			PlayMontageAndWaitTask->ReadyForActivation();
		}
	}
}

void UStereoMixGameplayAbility_Stun::OnComplete()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UStereoMixGameplayAbility_Stun::OnInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UStereoMixGameplayAbility_Stun::OnCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
