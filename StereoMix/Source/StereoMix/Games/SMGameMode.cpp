// Fill out your copyright notice in the Description page of Project Settings.


#include "SMGameMode.h"

#include "SMGameState.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Utilities/SMLog.h"

ASMGameMode::ASMGameMode()
{
	bUseSeamlessTravel = true;
}

FString ASMGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	const FString ErrorMessage = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

	FString InNickname = UGameplayStatics::ParseOption(Options, TEXT("Nickname")).Left(20);
	if (InNickname.IsEmpty())
	{
		InNickname = FString::Printf(TEXT("%s%i"), *DefaultPlayerName.ToString(), NewPlayerController->PlayerState->GetPlayerId());
	}

	ChangeName(NewPlayerController, InNickname, false);

	return ErrorMessage;
}

void ASMGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	BindToGameState();
}

void ASMGameMode::StartMatch()
{
	Super::StartMatch();

	NET_LOG(this, Warning, TEXT("게임 시작"));

	const float OneSecond = GetWorldSettings()->GetEffectiveTimeDilation();

	// 남은 라운드 시간을 초기화해줍니다.
	SetRemainRoundTime(RoundTime);
	GetWorldTimerManager().SetTimer(RoundTimerHandle, this, &ASMGameMode::PerformRoundTime, OneSecond, true);

	// 남은 페이즈 시간을 초기화해줍니다.
	SetRemainPhaseTime(PhaseTime);
	SetCurrentPhaseNumber(1);
	GetWorldTimerManager().SetTimer(PhaseTimerHandle, this, &ASMGameMode::PerformPhaseTime, OneSecond, true);
}

void ASMGameMode::EndMatch()
{
	// 남은 시간을 승패 확인 시간으로 다시 초기화시킵니다.
	SetRemainRoundTime(VictoryDefeatTime);

	// 매치가 종료되어 스코어 계산을 합니다.
	if (ensure(CachedSMGameState.Get()))
	{
		CachedSMGameState->EndRoundVictoryDefeatResult();
	}

	Super::EndMatch();
}

void ASMGameMode::BindToGameState()
{
	CachedSMGameState = GetGameState<ASMGameState>();
	if (ensureAlways(CachedSMGameState.Get()))
	{
		CachedSMGameState->SetReplicatedPhaseTime(PhaseTime);
	}
}

void ASMGameMode::EndVictoryDefeatTimer()
{
	// TODO: 임시로 현재 레벨을 재시작하도록 구성했습니다. 
	FString CurrentLevelPath = TEXT("/Game/StereoMix/Levels/Main/L_Main");
	GetWorld()->ServerTravel(CurrentLevelPath);
}

void ASMGameMode::SetRemainRoundTime(int32 InRemainRoundTime)
{
	RemainRoundTime = InRemainRoundTime;

	// 게임 스테이트로 복제합니다.
	if (ensure(CachedSMGameState.Get()))
	{
		CachedSMGameState->SetReplicatedRemainRoundTime(RemainRoundTime);
	}
}

void ASMGameMode::PerformRoundTime()
{
	// 라운드 타이머 종료 시
	if (!bIsRoundEnd && RemainRoundTime <= 0)
	{
		bIsRoundEnd = true;
		EndMatch();
		return;
	}

	// 승패 확인 타이머 종료 시
	if (bIsRoundEnd && RemainRoundTime <= 0)
	{
		EndVictoryDefeatTimer();
		return;
	}

	// 매 초 남은 시간을 감소시킵니다.
	SetRemainRoundTime(RemainRoundTime - 1);
}

void ASMGameMode::SetRemainPhaseTime(int32 InRemainPhaseTime)
{
	RemainPhaseTime = InRemainPhaseTime;

	// 게임 스테이트로 복제합니다.
	if (ensure(CachedSMGameState.Get()))
	{
		CachedSMGameState->SetReplicatedRemainPhaseTime(RemainPhaseTime);
	}
}

void ASMGameMode::PerformPhaseTime()
{
	SetRemainPhaseTime(RemainPhaseTime - 1);

	if (RemainPhaseTime <= 0)
	{
		// 한 페이즈 종료시 마다 처리됩니다. 
		SetCurrentPhaseNumber(CurrentPhaseNumber + 1);
		SetRemainPhaseTime(PhaseTime);

		if (ensureAlways(CachedSMGameState.Get()))
		{
			CachedSMGameState->EndPhase();
		}
	}
}

void ASMGameMode::SetCurrentPhaseNumber(int32 InCurrentPhaseNumber)
{
	CurrentPhaseNumber = InCurrentPhaseNumber;

	if (ensureAlways(CachedSMGameState.Get()))
	{
		// 게임 스테이트에 복제해줍니다.
		CachedSMGameState->SetReplicatedCurrentPhaseNumber(CurrentPhaseNumber);
	}
}
