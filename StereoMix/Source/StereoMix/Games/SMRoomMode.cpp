// Copyright Surround, Inc. All Rights Reserved.


#include "SMRoomMode.h"

#include "SMRoomState.h"
#include "Player/SMRoomPlayerState.h"
#include "Session/SMGameSession.h"
#include "Session/SMRoomSession.h"
#include "Utilities/SMLog.h"

ASMRoomMode::ASMRoomMode()
{
	bUseSeamlessTravel = true;

	GameSessionClass = ASMGameSession::StaticClass();
	GameStateClass = ASMRoomState::StaticClass();
	PlayerStateClass = ASMRoomPlayerState::StaticClass();

	CountdownTime = 5;
}

void ASMRoomMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
}

void ASMRoomMode::InitGameState()
{
	Super::InitGameState();
}

bool ASMRoomMode::IsReadyToStart()
{
	const int32 NumPlayers = GetNumPlayers();
	// const int32 MaxPlayers = GameSession->MaxPlayers; // 게임세션의 MaxPlayers가 제대로 적용되지 않음
	const int32 MaxPlayers = 6;
	NET_LOG(this, Verbose, TEXT("NumPlayers: %d, MaxPlayers: %d"), NumPlayers, MaxPlayers);
	return NumPlayers == MaxPlayers;
}

void ASMRoomMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
	UpdateRoomState();
}

void ASMRoomMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	UpdateRoomState();
}

void ASMRoomMode::UpdateRoomState()
{
	NET_LOG(this, Log, TEXT("[SMRoomMode] UpdateRoomState"));
	ASMRoomState* RoomState = Cast<ASMRoomState>(GameState);
	check(RoomState)

	if (const bool IsReady = IsReadyToStart(); IsReady && !CountdownTimerHandle.IsValid())
	{
		NET_LOG(this, Log, TEXT("[SMRoomMode] IsReadyToStart"))
		RoomState->Countdown = CountdownTime;
		RoomState->StartCountdown();
		GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &ASMRoomMode::CountdownTick, 1.0f, true);
	}
	else if (!IsReady && CountdownTimerHandle.IsValid())
	{
		NET_LOG(this, Log, TEXT("[SMRoomMode] Cancel Countdown"))
		GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
		RoomState->CancelCountdown();
	}
}

void ASMRoomMode::CountdownTick()
{
	ASMRoomState* RoomState = Cast<ASMRoomState>(GameState);
	check(RoomState)

	RoomState->Countdown--;

	if (RoomState->Countdown <= 0)
	{
		if (!IsReadyToStart())
		{
			RoomState->CancelCountdown();
			GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
		}
		else
		{
			GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
			StartGame();
		}
	}
	else
	{
		NET_LOG(this, Log, TEXT("Countdown: %d"), RoomState->Countdown);
	}
}

void ASMRoomMode::StartGame()
{
	ProcessServerTravel("/Game/StereoMix/Levels/CharacterSelect/L_CharacterSelect");
}
