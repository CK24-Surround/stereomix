// Copyright Surround, Inc. All Rights Reserved.


#include "SMRoomMode.h"

#include "CountdownTimerComponent.h"
#include "SMRoomState.h"
#include "StereoMix.h"
#include "Kismet/GameplayStatics.h"
#include "Player/SMRoomPlayerState.h"
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
		RoomState->ReplicatedMaxPlayersInGame = GameSession->MaxPlayers;
		RoomState->ReplicatedMaxPlayersInTeam = RoomState->ReplicatedMaxPlayersInGame / 2;
		UE_LOG(LogStereoMix, Verbose, TEXT("[SMRoomMode] MaxPlayers: %d, MaxPlayersInTeam: %d"), RoomState->ReplicatedMaxPlayersInGame, RoomState->ReplicatedMaxPlayersInTeam);
	}
	else
	{
		UE_LOG(LogStereoMix, Error, TEXT("[SMRoomMode] GameSession is not valid"))
	
		// Default values
		RoomState->ReplicatedMaxPlayersInGame = 6;
		RoomState->ReplicatedMaxPlayersInTeam = 3;
	}
	
	RoomState->GetCountdownTimer()->OnCountdownTick.AddDynamic(this, &ASMRoomMode::OnCountdownTick);
	RoomState->GetCountdownTimer()->OnCountdownFinished.AddDynamic(this, &ASMRoomMode::OnCountdownFinished);
	RoomState->OnTeamPlayersUpdated.AddDynamic(this, &ASMRoomMode::OnTeamPlayersUpdated);
	
	if (const FString RoomShortCode = FPlatformMisc::GetEnvironmentVariable(TEXT("STEREOMIX_SHORT_ROOM_ID")); !RoomShortCode.IsEmpty())
	{
		UE_LOG(LogStereoMix, Log, TEXT("STEREOMIX_SHORT_ROOM_ID: %s"), *RoomShortCode)
		RoomState->ShortRoomCode = RoomShortCode;
	}
	else
	{
		UE_LOG(LogStereoMix, Error, TEXT("environment variable 'STEREOMIX_SHORT_ROOM_ID' is not set"));
	}
}

FString ASMRoomMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
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

void ASMRoomMode::StartPlay()
{
	Super::StartPlay();
}

bool ASMRoomMode::IsReadyToStart() const
{
	if (!RoomState.IsValid())
	{
		return false;
	}

	const int32 EdmPlayersCount = RoomState->GetTeamEdmPlayers().Num();
	const int32 FutureBassPlayersCount = RoomState->GetTeamFutureBassPlayers().Num();
	const bool bReadyToStart = EdmPlayersCount == RoomState->ReplicatedMaxPlayersInTeam && FutureBassPlayersCount == RoomState->ReplicatedMaxPlayersInTeam;

	UE_LOG(LogStereoMix, Verbose,
	       TEXT("[SMRoomMode] IsReadyToStart: %s (EDM: %d, FutureBass: %d)"),
	       bReadyToStart ? TEXT("true") : TEXT("false"),
	       EdmPlayersCount,
	       FutureBassPlayersCount)

	return bReadyToStart;
}

void ASMRoomMode::OnTeamPlayersUpdated(ESMTeam UpdatedTeam)
{
	if (IsReadyToStart())
	{
		if (RoomState.IsValid())
		{
			RoomState->GetCountdownTimer()->StartCountdown(RoomState->CountdownTime);
		}
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
