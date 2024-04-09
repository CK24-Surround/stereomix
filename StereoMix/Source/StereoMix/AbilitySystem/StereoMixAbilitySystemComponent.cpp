// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixAbilitySystemComponent.h"

#include "Characters/StereoMixPlayerCharacter.h"
#include "Net/UnrealNetwork.h"

void UStereoMixAbilitySystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UStereoMixAbilitySystemComponent, CurrentHoldingPawn);
	DOREPLIFETIME(UStereoMixAbilitySystemComponent, CurrentHolderPawn);
}

void UStereoMixAbilitySystemComponent::OnTagUpdated(const FGameplayTag& Tag, bool TagExists)
{
	OnChangedTag.Broadcast(Tag, TagExists);
}
