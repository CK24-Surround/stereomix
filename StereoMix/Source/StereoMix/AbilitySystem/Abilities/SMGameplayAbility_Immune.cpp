// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayAbility_Immune.h"

#include "AbilitySystem/SMTags.h"

USMGameplayAbility_Immune::USMGameplayAbility_Immune()
{
	AbilityTags = FGameplayTagContainer(SMTags::Ability::Immune);

	ActivationOwnedTags = FGameplayTagContainer(SMTags::Character::State::Immune);
}

void USMGameplayAbility_Immune::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}
