// Fill out your copyright notice in the Description page of Project Settings.


#include "SMGameplayAbility_Caught.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Utilities/SMTags.h"

USMGameplayAbility_Caught::USMGameplayAbility_Caught()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	AbilityTags = FGameplayTagContainer(SMTags::Ability::Caught);
}

void USMGameplayAbility_Caught::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
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

	CommitAbility(Handle, ActorInfo, ActivationInfo);

	if (ActorInfo->IsNetAuthority())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
	}
}
