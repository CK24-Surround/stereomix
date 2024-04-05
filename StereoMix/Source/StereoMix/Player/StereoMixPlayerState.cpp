// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixPlayerState.h"

#include "AbilitySystemComponent.h"

AStereoMixPlayerState::AStereoMixPlayerState()
{
	NetUpdateFrequency = 30.0f;

	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
}

UAbilitySystemComponent* AStereoMixPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}
