// Copyright Surround, Inc. All Rights Reserved.


#include "SMRoomMode.h"

#include "SMRoomState.h"
#include "HUD/SMRoomHUD.h"
#include "Player/SMRoomPlayerState.h"
#include "Session/SMRoomSession.h"

ASMRoomMode::ASMRoomMode()
{
	bUseSeamlessTravel = true;

	GameSessionClass = ASMRoomSession::StaticClass();
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
	return GetNumPlayers() == GameSession->MaxPlayers;
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
	ASMRoomState* RoomState = Cast<ASMRoomState>(GameState);
	check(RoomState)

	if (const bool IsReady = IsReadyToStart(); IsReady && !CountdownTimerHandle.IsValid())
	{
		RoomState->StartCountdown();
		GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &ASMRoomMode::CountdownTick, 1.0f, true);
	}
	else if (!IsReady && CountdownTimerHandle.IsValid())
	{
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
			
		}
	}
}

void ASMRoomMode::StartGame()
{
	ProcessServerTravel("/Game/StereoMix/Levels/CharacterSelect/L_CharacterSelect");
}
