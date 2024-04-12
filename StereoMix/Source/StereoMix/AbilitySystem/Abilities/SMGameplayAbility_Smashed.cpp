// Fill out your copyright notice in the Description page of Project Settings.


#include "SMGameplayAbility_Smashed.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Characters/SMPlayerCharacter.h"
#include "Utilities/SMLog.h"
#include "Utilities/SMTagName.h"

USMGameplayAbility_Smashed::USMGameplayAbility_Smashed()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	AbilityTags = FGameplayTagContainer(FGameplayTag::RequestGameplayTag(SMTagName::Ability::Smashed));

	BuzzerBeaterSmashEndEventTag = FGameplayTag::RequestGameplayTag(SMTagName::Event::Character::BuzzerBeaterSmashEnd);
}

void USMGameplayAbility_Smashed::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	USMAbilitySystemComponent* SourceASC = GetStereoMixAbilitySystemComponentFromActorInfo();
	if (!ensure(SourceASC))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	auto SourceCharacter = GetStereoMixPlayerCharacterFromActorInfo();
	if (!ensure(SourceCharacter))
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

	// 버저 비터 종료 이벤트를 타겟에게 보냅니다. 이 이벤트는 만약 스턴 종료시간에 임박했을때 스매시를 시전하는 경우 스매시가 끊기는 것을 막고자 스턴 종료를 보류하게됩니다. 이 상황에서 스턴 종료 로직을 재개 시킬때 호출되어야합니다.
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(SourceCharacter, BuzzerBeaterSmashEndEventTag, FGameplayEventData());
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
