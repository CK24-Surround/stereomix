// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixAbilitySystemComponent.h"

#include "Characters/StereoMixPlayerCharacter.h"

void UStereoMixAbilitySystemComponent::OnTagUpdated(const FGameplayTag& Tag, bool TagExists)
{
	OnChangedTag.Broadcast(Tag, TagExists);
}
