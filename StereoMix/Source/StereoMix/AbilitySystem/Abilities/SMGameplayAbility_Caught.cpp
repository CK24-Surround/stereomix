// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayAbility_Caught.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/SMPlayerCharacter.h"

USMGameplayAbility_Caught::USMGameplayAbility_Caught()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	AbilityTags = FGameplayTagContainer(SMTags::Ability::Caught);
}

void USMGameplayAbility_Caught::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!ensureAlways(SourceCharacter))
	{
		EndAbilityByCancel();
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensure(SourceASC))
	{
		EndAbilityByCancel();
		return;
	}

	CachedCaughtMontage = CaughtMontage.FindOrAdd(SourceCharacter->GetTeam(), nullptr);

	// 재생을 시도하고 재생에 실패했다면 bWasCancelled = true로 종료합니다.
	const float Duration = SourceASC->PlayMontage(this, ActivationInfo, CachedCaughtMontage, 1.0f);
	if (!ensure(Duration > 0.0f))
	{
		EndAbilityByCancel();
		return;
	}

	CommitAbility(Handle, ActorInfo, ActivationInfo);

	K2_EndAbilityLocally();
}
