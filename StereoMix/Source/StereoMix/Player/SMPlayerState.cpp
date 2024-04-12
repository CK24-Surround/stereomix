// Fill out your copyright notice in the Description page of Project Settings.


#include "SMPlayerState.h"

#include "AbilitySystem/StereoMixAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSets/StereoMixCharacterAttributeSet.h"

ASMPlayerState::ASMPlayerState()
{
	NetUpdateFrequency = 30.0f;

	ASC = CreateDefaultSubobject<UStereoMixAbilitySystemComponent>(TEXT("ASC"));
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	CharacterAttributeSet = CreateDefaultSubobject<UStereoMixCharacterAttributeSet>(TEXT("CharacterAttributeSet"));
}

UAbilitySystemComponent* ASMPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}
