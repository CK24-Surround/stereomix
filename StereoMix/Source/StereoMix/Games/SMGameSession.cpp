// Fill out your copyright notice in the Description page of Project Settings.


#include "Games/SMGameSession.h"

#include "GameLiftServerSDK.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Player/SMPlayerState.h"

#if WITH_GAMELIFT
#include "API/GameLift.h"
#endif

ASMGameSession::ASMGameSession()
{
}

void ASMGameSession::RegisterServer()
{
	Super::RegisterServer();
	UE_LOG(LogSMGameSession, Log, TEXT("RegisterServer"));

#if WITH_GAMELIFT
	UE_LOG(LogSMGameSession, Log, TEXT("Initializing GameLift..."));

	if (FParse::Param(FCommandLine::Get(), TEXT("gamelift")))
	{
		UE_LOG(LogSMGameSession, Log, TEXT("Initializing GameLift..."));

		UGameLift* GameLiftModule = GetGameInstance()->GetSubsystem<UGameLift>();
		GameLiftModule->InitSDK();
	}
	else
	{
		UE_LOG(LogSMGameSession, Warning, TEXT("GameLift is not enabled."));
	}

#else
	UE_LOG(LogSMGameSession, Warning, TEXT("GameLift is not enabled."));
#endif
}

void ASMGameSession::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogSMGameSession, Log, TEXT("GameSession BeginPlay"));
}

void ASMGameSession::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UE_LOG(LogSMGameSession, Log, TEXT("GameSession EndPlay"));

#if WITH_GAMELIFT
	if (FGameLiftServerSDKModule* GameLiftSDK = GetGameInstance()->GetSubsystem<UGameLift>()->GetSDK())
	{
		GameLiftSDK->ProcessEnding();
	}
#endif
}

FString ASMGameSession::ApproveLogin(const FString& Options)
{
	UE_LOG(LogSMGameSession, Log, TEXT("ApproveLogin: %s"), *Options);

#if WITH_GAMELIFT
	FGameLiftServerSDKModule* GameLiftSDK = GetGameInstance()->GetSubsystem<UGameLift>()->GetSDK();
	check(GameLiftSDK);

	const FGameLiftStringOutcome GetGameSessionIdOutcome = GameLiftSDK->GetGameSessionId();
	// Check GameLift GameSession is initialized

	// Options="PlayerSessionId=[PlayerSessionId]"
	const FString PlayerSessionId = UGameplayStatics::ParseOption(Options, TEXT("PlayerSessionId"));
	if (PlayerSessionId.IsEmpty())
	{
		return TEXT("New player has no PlayerSessionId.");
	}

	UE_LOG(LogSMGameSession, Log, TEXT("Accepting new GameLift player session: %s"), *PlayerSessionId);
	if (const FGameLiftGenericOutcome AcceptPlayerSessionOutcome = GameLiftSDK->AcceptPlayerSession(PlayerSessionId);
		!AcceptPlayerSessionOutcome.IsSuccess())
	{
		UE_LOG(LogSMGameSession, Error,
		       TEXT("Failed to accept player session: %s"), *AcceptPlayerSessionOutcome.GetError().m_errorMessage);

		return FString::Printf(
			TEXT("GameLift AcceptPlayerSession failed: %s"), *AcceptPlayerSessionOutcome.GetError().m_errorMessage);
	}

	UE_LOG(LogSMGameSession, Log, TEXT("Player session accepted: %s"), *PlayerSessionId);

#else
	UE_LOG(LogSMGameSession, Warning, TEXT("GameLift is not enabled. Login will always succeed."));

#endif

	return Super::ApproveLogin(Options);
}

void ASMGameSession::UnregisterPlayer(const APlayerController* ExitingPlayer)
{
	Super::UnregisterPlayer(ExitingPlayer);

#if WITH_GAMELIFT
	FGameLiftServerSDKModule* GameLiftSDK = GetGameInstance()->GetSubsystem<UGameLift>()->GetSDK();
	check(GameLiftSDK);

	const FString PlayerSessionId = ExitingPlayer->GetPlayerState<ASMPlayerState>()->GetGameLiftPlayerSessionId();
	if (const FGameLiftGenericOutcome RemovePlayerSessionOutcome = GameLiftSDK->RemovePlayerSession(PlayerSessionId);
		!RemovePlayerSessionOutcome.IsSuccess())
	{
		UE_LOG(LogSMGameSession, Error, TEXT("Failed to remove player(%s) session: %s"),
		       *PlayerSessionId, *RemovePlayerSessionOutcome.GetError().m_errorMessage);
		return;
	}
	UE_LOG(LogSMGameSession, Log, TEXT("Player session removed: %s"), *PlayerSessionId);
#endif
}

bool ASMGameSession::HandleStartMatchRequest()
{
	UE_LOG(LogSMGameSession, Log, TEXT("HandleStartMatchRequest"));
	return Super::HandleStartMatchRequest();
}

void ASMGameSession::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
	UE_LOG(LogSMGameSession, Log, TEXT("HandleMatchHasEnded"));
}

void ASMGameSession::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	UE_LOG(LogSMGameSession, Log, TEXT("HandleMatchHasStarted"));
}
