// Fill out your copyright notice in the Description page of Project Settings.


#include "SMPlayerState.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSets/SMCharacterAttributeSet.h"
#include "Net/UnrealNetwork.h"

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

void ASMPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASMPlayerState, GameLiftPlayerId);
	DOREPLIFETIME_CONDITION(ASMPlayerState, GameLiftPlayerSessionId, COND_OwnerOnly);
}
