// Fill out your copyright notice in the Description page of Project Settings.


#include "SMAbilitySystemComponent.h"

#include "Characters/SMPlayerCharacter.h"

void USMAbilitySystemComponent::OnTagUpdated(const FGameplayTag& Tag, bool TagExists)
{
	OnChangedTag.Broadcast(Tag, TagExists);
}

void USMAbilitySystemComponent::AddTag(const FGameplayTag& InGameplayTag)
{
	AddLooseGameplayTag(InGameplayTag);
	AddReplicatedLooseGameplayTag(InGameplayTag);
}

void USMAbilitySystemComponent::RemoveTag(const FGameplayTag& InGameplayTag)
{
	if (HasMatchingGameplayTag(InGameplayTag))
	{
		RemoveLooseGameplayTag(InGameplayTag);
		RemoveReplicatedLooseGameplayTag(InGameplayTag);
	}
}
