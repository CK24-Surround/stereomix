// Copyright Surround, Inc. All Rights Reserved.


#include "SMGamePlayerState.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSets/SMCharacterAttributeSet.h"

ASMGamePlayerState::ASMGamePlayerState()
{
	NetUpdateFrequency = 30.0f;

	ASC = CreateDefaultSubobject<USMAbilitySystemComponent>(TEXT("ASC"));
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	CharacterAttributeSet = CreateDefaultSubobject<USMCharacterAttributeSet>(TEXT("CharacterAttributeSet"));
}

UAbilitySystemComponent* ASMGamePlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}
