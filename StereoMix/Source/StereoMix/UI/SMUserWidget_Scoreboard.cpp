// Fill out your copyright notice in the Description page of Project Settings.


#include "SMUserWidget_Scoreboard.h"

#include "AbilitySystemComponent.h"
#include "Components/TextBlock.h"
#include "Games/SMGameState.h"

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
	Timer->SetText(FText::FromString(FString::FromInt(RoundTime)));
}
