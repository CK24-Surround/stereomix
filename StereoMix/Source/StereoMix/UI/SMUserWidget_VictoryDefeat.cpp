// Fill out your copyright notice in the Description page of Project Settings.


#include "SMUserWidget_VictoryDefeat.h"

void USMUserWidget_VictoryDefeat::SetASC(UAbilitySystemComponent* InASC)
{
	Super::SetASC(InASC);

	if (!ensure(InASC))
	{
		return;
	}
}
