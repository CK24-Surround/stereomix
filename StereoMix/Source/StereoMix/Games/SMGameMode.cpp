// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameMode.h"

#include "GameFramework/PlayerState.h"
#include "GameMapsSettings.h"
#include "GameInstance/SMGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "SMGameState.h"
#include "Controllers/SMPlayerController.h"
#include "Session/SMGameSession.h"
#include "Utilities/SMLog.h"

ASMGameMode::ASMGameMode()
{
	bDelayedStart = true;
	bUseSeamlessTravel = true;
	GameSessionClass = ASMGameSession::StaticClass();
}

void ASMGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	RoomSession = Cast<ASMGameSession>(GameSession);
}

void ASMGameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
	for (const APlayerState* TargetPlayerState : GameState->PlayerArray)
	{
		ASMPlayerController* TargetPlayerController = Cast<ASMPlayerController>(TargetPlayerState->GetOwner());
		TargetPlayerController->ClientReceiveChat(TEXT("시스템"), FString::Printf(TEXT("%s 님이 입장했습니다."), *NewPlayer->PlayerState->GetPlayerName()));
	}
}

void ASMGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	for (const APlayerState* TargetPlayerState : GameState->PlayerArray)
	{
		ASMPlayerController* TargetPlayerController = Cast<ASMPlayerController>(TargetPlayerState->GetOwner());
		TargetPlayerController->ClientReceiveChat(TEXT("시스템"), FString::Printf(TEXT("%s 님이 나갔습니다."), *Exiting->PlayerState->GetPlayerName()));
	}
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

void ASMGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	NET_LOG(this, Warning, TEXT("게임 시작"));
	OnStartMatch.Broadcast();
}

void ASMGameMode::HandleLeavingMap()
{
	Super::HandleLeavingMap();

	if (bReturnToDefaultMapWhenGameEnds)
	{
		ProcessServerTravel(UGameMapsSettings::GetGameDefaultMap());
		return;
	}

	const USMGameInstance* GameInstance = GetGameInstance<USMGameInstance>();
	if (GameInstance->IsCustomGame())
	{
		ProcessServerTravel("/Game/StereoMix/Levels/Room/L_Room");
	}
	else
	{
		// 클라이언트들을 연결 종료시키고 서버를 종료합니다.
		ReturnToMainMenuHost();

		if (ensure(RoomSession.IsValid()) && RoomSession->IsConnectedWithBackend())
		{
			RoomSession->DeleteRoom();
		}
	}
}
