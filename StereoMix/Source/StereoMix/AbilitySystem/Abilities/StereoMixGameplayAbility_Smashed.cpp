// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixGameplayAbility_Smashed.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilitySystem/StereoMixAbilitySystemComponent.h"
#include "Utilities/StereoMixTag.h"

UStereoMixGameplayAbility_Smashed::UStereoMixGameplayAbility_Smashed()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	AbilityTags = FGameplayTagContainer(StereoMixTag::Ability::Smashed);
}

void UStereoMixGameplayAbility_Smashed::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UStereoMixAbilitySystemComponent* SourceASC = GetStereoMixAbilitySystemComponentFromActorInfo();
	if (ensure(SourceASC))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	const float Duration = SourceASC->PlayMontage(this, ActivationInfo, SmashedMontage, 1.0f);
	if (!ensure(Duration > 0.0f))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
