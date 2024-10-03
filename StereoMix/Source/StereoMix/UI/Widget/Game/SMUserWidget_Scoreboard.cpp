// Copyright Surround, Inc. All Rights Reserved.


#include "SMUserWidget_Scoreboard.h"

#include "AbilitySystemComponent.h"
#include "Components/TextBlock.h"
#include "Games/SMGameState.h"
#include "SMUserWidget_PhaseTimerBar.h"
#include "Components/Core/SMRoundTimerComponent.h"

USMUserWidget_Scoreboard::USMUserWidget_Scoreboard() {}

bool USMUserWidget_Scoreboard::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	TArray<TWeakObjectPtr<UImage>> EDMTeamPhases;
	EDMTeamPhases.AddUnique(EDMPhaseScore1);
	EDMTeamPhases.AddUnique(EDMPhaseScore2);

	TArray<TWeakObjectPtr<UImage>> FutureBassTeamPhases;
	FutureBassTeamPhases.AddUnique(FutureBassPhase1);
	FutureBassTeamPhases.AddUnique(FutureBassPhase2);

	PhaseScores.Add(ESMTeam::EDM, EDMTeamPhases);
	PhaseScores.Add(ESMTeam::FutureBass, FutureBassTeamPhases);

	return true;
}

void USMUserWidget_Scoreboard::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdatePhaseTime();
}

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
	UWorld* World = GetWorld();
	ASMGameState* GameState = World ? World->GetGameState<ASMGameState>() : nullptr;
	if (GameState)
	{
		if (USMRoundTimerComponent* RoundTimerComponent = GameState->GetComponentByClass<USMRoundTimerComponent>())
		{
			RoundTimerComponent->OnRemainingRoundTimeChanged.AddDynamic(this, &ThisClass::OnRoundTimeChanged);
			OnRoundTimeChanged(RoundTimerComponent->GetRemainingTime());
		}

		// SMGameState->OnChangeEDMTeamScore.BindUObject(this, &USMUserWidget_Scoreboard::OnChangeEDMScore);
		// OnChangeEDMScore(SMGameState->GetReplicatedEDMTeamScore());
		//
		// SMGameState->OnChangeFutureBassTeamScore.BindUObject(this, &USMUserWidget_Scoreboard::OnChangeFutureBaseScore);
		// OnChangeFutureBaseScore(SMGameState->GetReplicatedFutureBaseTeamScore());
		//
		// SMGameState->OnChangePhaseTime.BindUObject(this, &USMUserWidget_Scoreboard::OnChangePhaseTime);
		// OnChangePhaseTime(SMGameState->GetReplicatedRemainPhaseTime(), SMGameState->GetReplicatedPhaseTime());
		//
		// SMGameState->OnChangePhase.BindUObject(this, &USMUserWidget_Scoreboard::OnChangePhase);
		// OnChangePhase(SMGameState->GetReplicatedCurrentPhaseNumber());
		//
		// SMGameState->OnChangeEDMTeamPhaseScore.BindUObject(this, &USMUserWidget_Scoreboard::OnChangeEDMPhaseScore);
		// OnChangeEDMPhaseScore(SMGameState->GetReplicatedEDMTeamPhaseScore());
		//
		// SMGameState->OnChangeFutureBassTeamPhaseScore.BindUObject(this, &USMUserWidget_Scoreboard::OnChangeFutureBassPhaseScore);
		// OnChangeFutureBassPhaseScore(SMGameState->GetReplicatedFutureBaseTeamPhaseScore());
	}
	else
	{
		// 만약 게임스테이트가 유효하지 않다면 바인드 시점을 다음 틱으로 미룹니다. 이는 성공적으로 바인드 될때까지 계속 수행됩니다.
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &USMUserWidget_Scoreboard::BindToGameState);
	}
}

void USMUserWidget_Scoreboard::OnRoundTimeChanged(int32 RoundTime)
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
	FutureBassScore->SetText(FText::FromString(ScoreString));
}

void USMUserWidget_Scoreboard::OnChangePhaseTime(int32 RemainPhaseTime, int32 PhaseTime)
{
	PhaseTimerBarRemainPhaseTime = RemainPhaseTime;
	PhaseTimerBarPhaseTime = PhaseTime;
	PhaseTimerBarElapsedTime = 0.0f;
}

void USMUserWidget_Scoreboard::UpdatePhaseTime()
{
	PhaseTimerBarElapsedTime += GetWorld()->GetDeltaSeconds();
	const float RemainTime = PhaseTimerBarRemainPhaseTime - PhaseTimerBarElapsedTime;
	const float Ratio = RemainTime / PhaseTimerBarPhaseTime;

	PhaseTimerBar->SetRatio(Ratio);
}

void USMUserWidget_Scoreboard::OnChangePhase(int32 CurrentPhaseNumber)
{
	const FString PhaseNumberString = FString::Printf(TEXT("Phase%d"), CurrentPhaseNumber);
	PhaseNumber->SetText(FText::FromString(PhaseNumberString));
}

void USMUserWidget_Scoreboard::OnChangeEDMPhaseScore(int32 Score)
{
	// 만약 페이즈 스코어 수가 UI상의 스코어 칸 개수보다 많으면 무시합니다.
	if (PhaseScores[ESMTeam::EDM].Num() < Score)
	{
		return;
	}

	// 순회하며 해당 스코어 수 만큼 활성화 시킵니다.
	for (int32 i = 0; i < Score; ++i)
	{
		PhaseScores[ESMTeam::EDM][i]->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void USMUserWidget_Scoreboard::OnChangeFutureBassPhaseScore(int32 Score)
{
	// 만약 페이즈 스코어 수가 UI상의 스코어 칸 개수보다 많으면 무시합니다.
	if (PhaseScores[ESMTeam::FutureBass].Num() < Score)
	{
		return;
	}

	// 순회하며 해당 스코어 수 만큼 활성화 시킵니다.
	for (int32 i = 0; i < Score; ++i)
	{
		PhaseScores[ESMTeam::FutureBass][i]->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}
