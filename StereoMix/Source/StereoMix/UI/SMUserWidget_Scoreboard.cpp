// Fill out your copyright notice in the Description page of Project Settings.


#include "SMUserWidget_Scoreboard.h"

#include "AbilitySystemComponent.h"
#include "Components/TextBlock.h"
#include "Games/SMGameState.h"

void USMUserWidget_Scoreboard::SetASC(UAbilitySystemComponent* InASC)
{
	Super::SetASC(InASC);

	if (!ensure(ASC.Get()))
	{
		return;
	}

	BindToGameState();
}

void USMUserWidget_Scoreboard::BindToGameState()
{
	ASMGameState* SMGameState = ASC->GetWorld()->GetGameState<ASMGameState>();
	if (SMGameState)
	{
		SMGameState->OnChangeRoundTime.BindUObject(this, &USMUserWidget_Scoreboard::OnChangeRoundTime);
		OnChangeRoundTime(SMGameState->GetRemainRoundTime());

		SMGameState->OnChangeEDMTeamScore.BindUObject(this, &USMUserWidget_Scoreboard::OnChangeEDMScore);
		OnChangeEDMScore(SMGameState->GetReplicatedEDMTeamScore());

		SMGameState->OnChangeFutureBaseTeamScore.BindUObject(this, &USMUserWidget_Scoreboard::OnChangeFutureBaseScore);
		OnChangeFutureBaseScore(SMGameState->GetReplicatedFutureBaseTeamScore());
	}
	else
	{
		// 만약 게임스테이트가 유효하지 않다면 바인드 시점을 다음 틱으로 미룹니다. 이는 성공적으로 바인드 될때까지 계속 수행됩니다.
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &USMUserWidget_Scoreboard::BindToGameState);
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
