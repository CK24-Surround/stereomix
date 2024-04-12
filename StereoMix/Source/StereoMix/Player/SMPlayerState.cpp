// Fill out your copyright notice in the Description page of Project Settings.


#include "SMPlayerState.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSets/SMCharacterAttributeSet.h"

ASMPlayerState::ASMPlayerState()
{
	NetUpdateFrequency = 30.0f;

	ASC = CreateDefaultSubobject<USMAbilitySystemComponent>(TEXT("ASC"));
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	CharacterAttributeSet = CreateDefaultSubobject<USMCharacterAttributeSet>(TEXT("CharacterAttributeSet"));
}

UAbilitySystemComponent* ASMPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}
