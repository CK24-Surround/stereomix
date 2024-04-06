// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixPlayerState.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystems/AttributeSets/StereoMixCharacterAttributeSet.h"

AStereoMixPlayerState::AStereoMixPlayerState()
{
	NetUpdateFrequency = 30.0f;

	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	CharacterAttributeSet = CreateDefaultSubobject<UStereoMixCharacterAttributeSet>(TEXT("CharacterAttributeSet"));
}

UAbilitySystemComponent* AStereoMixPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}
