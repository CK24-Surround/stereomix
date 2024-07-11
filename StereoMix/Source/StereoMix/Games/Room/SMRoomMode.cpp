// Copyright Surround, Inc. All Rights Reserved.


#include "SMRoomMode.h"

#include "GameInstance/SMGameInstance.h"
#include "Games/SMCountdownTimerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SMRoomPlayerState.h"
#include "SMRoomState.h"
#include "Session/SMGameSession.h"
#include "StereoMixLog.h"
#include "Controllers/SMPlayerController.h"

class ASMPlayerController;

ASMRoomMode::ASMRoomMode()
{
	bUseSeamlessTravel = true;

	GameSessionClass = ASMGameSession::StaticClass();
	GameStateClass = ASMRoomState::StaticClass();
	PlayerStateClass = ASMRoomPlayerState::StaticClass();
}

void ASMRoomMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (GetWorld() && EmptyRoomCheckTimerHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(EmptyRoomCheckTimerHandle);
	}
}

void ASMRoomMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	RoomSession = Cast<ASMGameSession>(GameSession);

	const USMGameInstance* GameInstance = GetGameInstance<USMGameInstance>();
	check(GameInstance)

	if (RoomSession.IsValid())
	{
		RoomSession->SetCanEnterRoom(true);
		if (!GameInstance->IsCustomGame())
		{
			GetWorldTimerManager().SetTimer(EmptyRoomCheckTimerHandle, this, &ASMRoomMode::EmptyRoomCheckTick, 10.f, true, 30.f);
		}
	}
}

void ASMRoomMode::InitGameState()
{
	Super::InitGameState();
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

	// 직접 게임시작으로 사양 변경됨
	// RoomState->GetCountdownTimer()->OnCountdownTick.AddDynamic(this, &ASMRoomMode::OnCountdownTick);
	// RoomState->GetCountdownTimer()->OnCountdownFinished.AddDynamic(this, &ASMRoomMode::OnCountdownFinished);

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

	FString InNickname = UGameplayStatics::ParseOption(Options, TEXT("Nickname")).Left(20);
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

	UE_LOG(LogStereoMix, Verbose, TEXT("[SMRoomMode] IsReadyToStart: %s (EDM: %d, FutureBass: %d)"), bReadyToStart ? TEXT("true") : TEXT("false"), EdmPlayersCount, FutureBassPlayersCount)

	return bReadyToStart;
}

void ASMRoomMode::OnTeamPlayersUpdated(ESMTeam UpdatedTeam)
{
	// if (RoomState.IsValid() && !RoomState->GetCountdownTimer()->IsRunning() && IsReadyToStart())
	// {
	// 	RoomState->GetCountdownTimer()->StartCountdown(RoomState->CountdownTime);
	// }
}

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
	StartGameIfReadyToStart();
}

void ASMRoomMode::EmptyRoomCheckTick()
{
	check(RoomSession.IsValid())
	if (GetNumPlayers() == 0 && RoomSession->IsConnectedWithBackend())
	{
		UE_LOG(LogStereoMix, Warning, TEXT("No players in this room. Room will close."))
		RoomSession->DeleteRoom();
	}
}

void ASMRoomMode::StartGame()
{
	if (RoomSession.IsValid())
	{
		RoomSession->UpdateRoomState(EGrpcLobbyRoomState::ROOM_STATE_PLAYING);
	}

	ProcessServerTravel("/Game/StereoMix/Levels/CharacterSelect/L_CharacterSelect");
}

bool ASMRoomMode::StartGameIfReadyToStart()
{
	if (IsReadyToStart())
	{
		StartGame();
		return true;
	}
	return false;
}

void ASMRoomMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	for (auto TargetPlayerState : GameState->PlayerArray)
	{
		ASMPlayerController* TargetPlayerController = Cast<ASMPlayerController>(TargetPlayerState->GetOwner());
		TargetPlayerController->ClientReceiveChat(TEXT("시스템"), FString::Printf(TEXT("%s 님이 입장했습니다."), *NewPlayer->PlayerState->GetPlayerName()));
	}
}

void ASMRoomMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	for (auto TargetPlayerState : GameState->PlayerArray)
	{
		ASMPlayerController* TargetPlayerController = Cast<ASMPlayerController>(TargetPlayerState->GetOwner());
		TargetPlayerController->ClientReceiveChat(TEXT("시스템"), FString::Printf(TEXT("%s 님이 나갔습니다."), *Exiting->PlayerState->GetPlayerName()));
	}
}
