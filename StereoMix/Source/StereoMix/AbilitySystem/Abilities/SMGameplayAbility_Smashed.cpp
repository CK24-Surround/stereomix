// Fill out your copyright notice in the Description page of Project Settings.


#include "SMGameplayAbility_Smashed.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Characters/SMPlayerCharacter.h"
#include "Utilities/SMTagName.h"

USMGameplayAbility_Smashed::USMGameplayAbility_Smashed()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	AbilityTags = FGameplayTagContainer(FGameplayTag::RequestGameplayTag(SMTagName::Ability::Smashed));

	OnSmashEventTag = FGameplayTag::RequestGameplayTag(SMTagName::Event::Character::OnSmash);
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

	// 스매시 공격 발생 이벤트를 타겟에게 보냅니다. 이 이벤트는 만약 스턴 시간이 다 되었을때 스매시를 시전한 경우 마무리하기 위해서 쓰입니다.
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(SourceCharacter, OnSmashEventTag, FGameplayEventData());
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
