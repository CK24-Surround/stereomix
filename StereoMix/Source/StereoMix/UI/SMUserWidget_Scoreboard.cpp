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
	NET_LOG(InASC->GetAvatarActor(), Warning, TEXT("게임 스테이트: %p"), SMGameState);
	if (SMGameState)
	{
		SMGameState->OnChangeRoundTime.BindUObject(this, &USMUserWidget_Scoreboard::OnChangeRoundTime);
		SMGameState->OnChangeEDMTeamScore.BindUObject(this, &USMUserWidget_Scoreboard::OnChangeEDMScore);
		SMGameState->OnChangeFutureBaseTeamScore.BindUObject(this, &USMUserWidget_Scoreboard::OnChangeFutureBaseScore);
	}
}

void USMUserWidget_Scoreboard::OnChangeRoundTime(int32 RoundTime)
{
	const int32 Minutes = RoundTime / 60;
	const int32 Seconds = RoundTime % 60;
	const FString RoundTimerString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
	RoundTimer->SetText(FText::FromString(RoundTimerString));
}

void USMUserWidget_Scoreboard::OnChangeEDMScore(int32 Score)
{
	const FString ScoreString = FString::Printf(TEXT("%03d"), Score);
	EDMScore->SetText(FText::FromString(ScoreString));
}

void USMUserWidget_Scoreboard::OnChangeFutureBaseScore(int32 Score)
{
	const FString ScoreString = FString::Printf(TEXT("%03d"), Score);
	FutureBaseScore->SetText(FText::FromString(ScoreString));
}
