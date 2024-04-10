// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixGameplayAbility_Caught.h"

#include "AbilitySystem/StereoMixAbilitySystemComponent.h"
#include "Utilities/StereoMixTag.h"

UStereoMixGameplayAbility_Caught::UStereoMixGameplayAbility_Caught()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

	AbilityTags = FGameplayTagContainer(StereoMixTag::Ability::Caught);
}

void UStereoMixGameplayAbility_Caught::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UStereoMixAbilitySystemComponent* SourceASC = GetStereoMixAbilitySystemComponentFromActorInfo();
	if (!ensure(SourceASC))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	// 재생을 시도하고 재생에 실패했다면 bWasCancelled = true로 종료합니다.
	const float Duration = SourceASC->PlayMontage(this, ActivationInfo, CaughtMontage, 1.0f);
	if (!ensure(Duration > 0.0f))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
