// Fill out your copyright notice in the Description page of Project Settings.


#include "SMUserWidget_HUD.h"

#include "SMUserWidget_Scoreboard.h"

void USMUserWidget_HUD::SetASC(UAbilitySystemComponent* InASC)
{
	Super::SetASC(InASC);

	Scoreboard->SetASC(InASC);
}
