// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixAbilitySystemComponent.h"

#include "Characters/SMPlayerCharacter.h"

void UStereoMixAbilitySystemComponent::OnTagUpdated(const FGameplayTag& Tag, bool TagExists)
{
	OnChangedTag.Broadcast(Tag, TagExists);
}

void UStereoMixAbilitySystemComponent::AddTag(const FGameplayTag& InGameplayTag)
{
	AddLooseGameplayTag(InGameplayTag);
	AddReplicatedLooseGameplayTag(InGameplayTag);
}

void UStereoMixAbilitySystemComponent::RemoveTag(const FGameplayTag& InGameplayTag)
{
	if (HasMatchingGameplayTag(InGameplayTag))
	{
		RemoveLooseGameplayTag(InGameplayTag);
		RemoveReplicatedLooseGameplayTag(InGameplayTag);
	}
}
