// Fill out your copyright notice in the Description page of Project Settings.


#include "SMUserWidget_Scoreboard.h"

#include "AbilitySystemComponent.h"
#include "Components/TextBlock.h"
#include "Games/SMGameState.h"
#include "Utilities/SMLog.h"

void USMUserWidget_Scoreboard::SetASC(UAbilitySystemComponent* InASC)
{
	Super::SetASC(InASC);

	ASMGameState* SMGameState = InASC->GetWorld()->GetGameState<ASMGameState>();
	if (ensure(SMGameState))
	{
		SMGameState->OnChangeRoundTime.BindUObject(this, &USMUserWidget_Scoreboard::OnChangeRoundTime);
	}
}

void USMUserWidget_Scoreboard::OnChangeRoundTime(int32 RoundTime)
{
	const int32 Minutes = RoundTime / 60;
	const int32 Seconds =  RoundTime % 60;
	const FString RoundTimerString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds); 
	Timer->SetText(FText::FromString(RoundTimerString));
}
