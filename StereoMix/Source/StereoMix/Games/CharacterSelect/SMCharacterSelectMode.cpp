// Copyright Surround, Inc. All Rights Reserved.


#include "SMCharacterSelectMode.h"

#include "Controllers/SMCharacterSelectPlayerController.h"
#include "SMCharacterSelectPlayerState.h"
#include "SMCharacterSelectState.h"
#include "Session/SMGameSession.h"
#include "StereoMixLog.h"
#include "GameInstance/SMGameInstance.h"
#include "Games/SMCountdownTimerComponent.h"

ASMCharacterSelectMode::ASMCharacterSelectMode()
{
	bUseSeamlessTravel = true;
	GameSessionClass = ASMGameSession::StaticClass();
	GameStateClass = ASMCharacterSelectState::StaticClass();
	PlayerStateClass = ASMCharacterSelectPlayerState::StaticClass();
	PlayerControllerClass = ASMCharacterSelectPlayerController::StaticClass();
}

void ASMCharacterSelectMode::InitGameState()
{
	Super::InitGameState();
	CharacterSelectState = CastChecked<ASMCharacterSelectState>(GameState);
}

void ASMCharacterSelectMode::StartPlay()
{
	Super::StartPlay();

	RemainingWaitingTime = 30;
	GetWorldTimerManager().SetTimer(WaitingTimerHandle, this, &ASMCharacterSelectMode::WaitingTimerTick, 1.0f, true, 10.0f);
}

void ASMCharacterSelectMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void ASMCharacterSelectMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}

bool ASMCharacterSelectMode::IsAllPlayerReadyToSelect() const
{
	if (CharacterSelectState.IsValid())
	{
		for (TObjectPtr<APlayerState> PlayerState : CharacterSelectState->PlayerArray)
		{
			const ASMCharacterSelectPlayerState* CharacterSelectPlayerState = Cast<ASMCharacterSelectPlayerState>(PlayerState);
			if (CharacterSelectPlayerState && CharacterSelectPlayerState->GetCurrentState() == ECharacterSelectPlayerStateType::Loading)
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

void ASMCharacterSelectMode::ImmediateStart()
{
	if (!CharacterSelectState.IsValid() || CharacterSelectState->GetCurrentState() != ECharacterSelectionStateType::Select)
	{
		return;
	}

	CharacterSelectState->GetCountdownTimer()->CancelCountdown();
	OnCharacterSelectCountdownFinished();
}

void ASMCharacterSelectMode::WaitingTimerTick()
{
	RemainingWaitingTime--;
	if (IsAllPlayerReadyToSelect() || RemainingWaitingTime <= 0)
	{
		GetWorldTimerManager().ClearTimer(WaitingTimerHandle);
		UE_LOG(LogStereoMix, Verbose, TEXT("Character select countdown started."))
		CharacterSelectState->SetCurrentState(ECharacterSelectionStateType::Select);

		CharacterSelectState->GetCountdownTimer()->OnCountdownFinished.AddUniqueDynamic(this, &ASMCharacterSelectMode::OnCharacterSelectCountdownFinished);
		CharacterSelectState->GetCountdownTimer()->StartCountdown(CharacterSelectState->CharacterSelectCountdownTime);
		return;
	}

	UE_LOG(LogStereoMix, Verbose, TEXT("Waiting for players to join. %d seconds remaining."), RemainingWaitingTime)
}

void ASMCharacterSelectMode::OnCharacterSelectCountdownFinished()
{
	if (CharacterSelectState.IsValid())
	{
		// 아직 선택하지 않은 플레이어들은 기본 캐릭터로 설정
		for (TObjectPtr<APlayerState> PlayerState : CharacterSelectState->PlayerArray)
		{
			if (ASMCharacterSelectPlayerState* CharacterSelectPlayerState = Cast<ASMCharacterSelectPlayerState>(PlayerState))
			{
				if (CharacterSelectPlayerState->GetCharacterType() == ESMCharacterType::None)
				{
					// 임시로 기본 기타로 설정
					CharacterSelectPlayerState->ChangeCharacterType(ESMCharacterType::ElectricGuitar);
				}
				UE_LOG(LogStereoMix, Log, TEXT("Player %s automatically select character %s."), *CharacterSelectPlayerState->GetPlayerName(), *UEnum::GetValueAsString(CharacterSelectPlayerState->GetCharacterType()))
			}
		}

		// CharacterSelectState->GetCountdownTimer()->OnCountdownFinished.RemoveDynamic(this, &ASMCharacterSelectMode::OnCountdownFinished);
		CharacterSelectState->GetCountdownTimer()->OnCountdownFinished.AddUniqueDynamic(this, &ASMCharacterSelectMode::StartGame);

		UE_LOG(LogStereoMix, Verbose, TEXT("Game will start in 5 seconds."))
		for (auto TargetPlayerState : GameState->PlayerArray)
		{
			ASMPlayerController* TargetPlayerController = Cast<ASMPlayerController>(TargetPlayerState->GetOwner());
			TargetPlayerController->ClientReceiveChat(TEXT("시스템"), FString::Printf(TEXT("5초 뒤에 게임이 시작됩니다.")));
		}

		CharacterSelectState->SetCurrentState(ECharacterSelectionStateType::End);
		CharacterSelectState->GetCountdownTimer()->StartCountdown(5);
	}
}

void ASMCharacterSelectMode::StartGame()
{
	const UWorld* World = GetWorld();
	const USMGameInstance* GameInstance = World ? World->GetGameInstance<USMGameInstance>() : nullptr;
	const TArray<FSoftObjectPath>& StageLevels = GameInstance ? GameInstance->GetStageLevels() : TArray<FSoftObjectPath>();
	if (StageLevels.IsValidIndex(0))
	{
		ProcessServerTravel(StageLevels[0].GetLongPackageName());
	}
}
