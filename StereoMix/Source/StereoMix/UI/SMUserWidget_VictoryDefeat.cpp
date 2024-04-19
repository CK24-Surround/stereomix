// Fill out your copyright notice in the Description page of Project Settings.


#include "SMUserWidget_VictoryDefeat.h"

#include "Games/SMGameState.h"

void USMUserWidget_VictoryDefeat::SetASC(UAbilitySystemComponent* InASC)
{
	Super::SetASC(InASC);

	if (!ensure(InASC))
	{
		return;
	}

	BindToGameState();
}

void USMUserWidget_VictoryDefeat::BindToGameState()
{
	ASMGameState* SMGameState = GetWorld()->GetGameState<ASMGameState>();
	if (ensure(SMGameState))
	{
		SMGameState->OnEndRound.BindUObject(this, &USMUserWidget_VictoryDefeat::OnEndRound);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &USMUserWidget_VictoryDefeat::BindToGameState);
	}
}

void USMUserWidget_VictoryDefeat::OnEndRound() {}
