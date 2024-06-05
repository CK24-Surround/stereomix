// Copyright Surround, Inc. All Rights Reserved.


#include "SMRoomMode.h"

#include "SMRoomPlayerState.h"
#include "SMRoomState.h"
#include "StereoMix.h"
#include "StereoMixLog.h"
#include "Games/CountdownTimerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Session/SMRoomSession.h"

ASMRoomMode::ASMRoomMode()
{
	bUseSeamlessTravel = true;

	GameSessionClass = ASMRoomSession::StaticClass();
	GameStateClass = ASMRoomState::StaticClass();
	PlayerStateClass = ASMRoomPlayerState::StaticClass();
}

void ASMRoomMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	UE_LOG(LogTemp, Warning, TEXT("[SMRoomMode] InitGame"))
}

void ASMRoomMode::InitGameState()
{
	Super::InitGameState();
	UE_LOG(LogTemp, Warning, TEXT("[SMRoomMode] InitGameState"))
	RoomState = CastChecked<ASMRoomState>(GameState);

	if (GameSession)
	{
		RoomState->MaxPlayersInGame = GameSession->MaxPlayers;
		RoomState->MaxPlayersInTeam = RoomState->MaxPlayersInGame / 2;
		UE_LOG(LogStereoMix, Verbose, TEXT("[SMRoomMode] MaxPlayers: %d, MaxPlayersInTeam: %d"), RoomState->MaxPlayersInGame, RoomState->MaxPlayersInTeam);
	}
	else
	{
		UE_LOG(LogStereoMix, Error, TEXT("[SMRoomMode] GameSession is invalid"))

		// Default values
		RoomState->MaxPlayersInGame = 6;
		RoomState->MaxPlayersInTeam = 3;
	}

	RoomState->GetCountdownTimer()->OnCountdownTick.AddDynamic(this, &ASMRoomMode::OnCountdownTick);
	RoomState->GetCountdownTimer()->OnCountdownFinished.AddDynamic(this, &ASMRoomMode::OnCountdownFinished);
	RoomState->OnTeamPlayersUpdated.AddDynamic(this, &ASMRoomMode::OnTeamPlayersUpdated);

	const FString RoomCode = FPlatformMisc::GetEnvironmentVariable(TEXT("STEREOMIX_SHORT_ROOM_ID"));
	if (!RoomCode.IsEmpty())
	{
		UE_LOG(LogStereoMix, Log, TEXT("STEREOMIX_SHORT_ROOM_ID: %s"), *RoomCode)
	}
	else
	{
		UE_LOG(LogStereoMix, Error, TEXT("Environment variable 'STEREOMIX_SHORT_ROOM_ID' is not set"));
	}
	RoomState->SetRoomCode(RoomCode);
}

FString ASMRoomMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	const FString ErrorMessage = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

	FString InNickname = UGameplayStatics::ParseOption(Options, TEXT("Nickname")).Left(10);
	if (InNickname.IsEmpty())
	{
		InNickname = FString::Printf(TEXT("%s%i"), *DefaultPlayerName.ToString(), NewPlayerController->PlayerState->GetPlayerId());
	}

	ChangeName(NewPlayerController, InNickname, false);

	return ErrorMessage;
}

bool ASMRoomMode::IsReadyToStart() const
{
	if (!RoomState.IsValid())
	{
		return false;
	}

	const int32 EdmPlayersCount = RoomState->GetTeamEdmPlayers().Num();
	const int32 FutureBassPlayersCount = RoomState->GetTeamFutureBassPlayers().Num();
	const bool bReadyToStart = EdmPlayersCount == RoomState->MaxPlayersInTeam && FutureBassPlayersCount == RoomState->MaxPlayersInTeam;

	UE_LOG(LogStereoMix, Verbose,
	       TEXT("[SMRoomMode] IsReadyToStart: %s (EDM: %d, FutureBass: %d)"),
	       bReadyToStart ? TEXT("true") : TEXT("false"),
	       EdmPlayersCount,
	       FutureBassPlayersCount)

	return bReadyToStart;
}

void ASMRoomMode::OnTeamPlayersUpdated(ESMTeam UpdatedTeam)
{
	if (RoomState.IsValid() && !RoomState->GetCountdownTimer()->IsRunning() && IsReadyToStart())
	{
		RoomState->GetCountdownTimer()->StartCountdown(RoomState->CountdownTime);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ASMRoomMode::OnCountdownTick()
{
	if (!IsReadyToStart())
	{
		if (RoomState.IsValid())
		{
			RoomState->GetCountdownTimer()->CancelCountdown();
			UE_LOG(LogStereoMix, Verbose, TEXT("[SMRoomMode] Countdown canceled because not ready to start"))
		}
	}
}

void ASMRoomMode::OnCountdownFinished()
{
	if (IsReadyToStart())
	{
		StartGame();
	}
}

void ASMRoomMode::StartGame()
{
	ProcessServerTravel("/Game/StereoMix/Levels/CharacterSelect/L_CharacterSelect");
}
