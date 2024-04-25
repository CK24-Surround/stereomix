// Fill out your copyright notice in the Description page of Project Settings.

#include "Games/SMGameLiftSession.h"
#include "GameLiftServerSDK.h"

#if WITH_GAMELIFT
#include "Player/SMPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "API/GameLift.h"
#endif

ASMGameLiftSession::ASMGameLiftSession()
{
}

void ASMGameLiftSession::BeginPlay()
{
	Super::BeginPlay();
}

void ASMGameLiftSession::RegisterServer()
{
	Super::RegisterServer();
	UE_LOG(LogSMGameLiftSession, Verbose, TEXT("RegisterServer"));

#if WITH_GAMELIFT

	// Options: -gamelift
	if (FParse::Param(FCommandLine::Get(), TEXT("gamelift")))
	{
		UE_LOG(LogSMGameLiftSession, Log, TEXT("Initializing GameLift..."));
		UGameLift* GameLiftModule = GetGameInstance()->GetSubsystem<UGameLift>();
		GameLiftModule->InitSDK();
	}
	else
	{
		UE_LOG(LogSMGameLiftSession, Warning, TEXT("GameLift is not enabled."));
	}

#else
	UE_LOG(LogSMGameLiftSession, Warning, TEXT("GameLift is not enabled."));
#endif
}

void ASMGameLiftSession::RegisterServerFailed()
{
	Super::RegisterServerFailed();
	UE_LOG(LogSMGameLiftSession, Warning, TEXT("RegisterServerFailed"));
}

#if WITH_GAMELIFT
void ASMGameLiftSession::OnTerminateFromGameLift()
{
	UE_LOG(LogSMGameLiftSession, Log, TEXT("OnTerminateFromGameLift"));
}
#endif

FString ASMGameLiftSession::ApproveLogin(const FString& Options)
{
	UE_LOG(LogSMGameLiftSession, Log, TEXT("ApproveLogin: %s"), *Options);

#if WITH_GAMELIFT
	FGameLiftServerSDKModule* GameLiftSDK = GetGameInstance()->GetSubsystem<UGameLift>()->GetSDK();
	check(GameLiftSDK);

	if (FParse::Param(FCommandLine::Get(), TEXT("gamelift")))
	{
		// Options: PlayerSessionId=psess-1234567890
		const FString PlayerSessionId = UGameplayStatics::ParseOption(Options, TEXT("PlayerSessionId"));
		if (PlayerSessionId.IsEmpty())
		{
			return TEXT("New player has no PlayerSessionId.");
		}

		UE_LOG(LogSMGameLiftSession, Log, TEXT("Accepting new GameLift player session: %s"), *PlayerSessionId);
		const FGameLiftGenericOutcome AcceptPlayerSessionOutcome = GameLiftSDK->AcceptPlayerSession(PlayerSessionId);
		if (!AcceptPlayerSessionOutcome.IsSuccess())
		{
			const FString AcceptPlayerError = FString::Printf(
				TEXT("Failed to accept player session: %s"), *AcceptPlayerSessionOutcome.GetError().m_errorMessage);

			UE_LOG(LogSMGameLiftSession, Error, TEXT("%s"), *AcceptPlayerError);
			return AcceptPlayerError;
		}

		UE_LOG(LogSMGameLiftSession, Log, TEXT("Player session accepted: %s"), *PlayerSessionId);
	}
	else
	{
		UE_LOG(LogSMGameLiftSession, Warning, TEXT("GameLift is not enabled. Login will always succeed."));
	}

#else
	UE_LOG(LogSMGameLiftSession, Warning, TEXT("GameLift is not enabled. Login will always succeed."));

#endif

	return Super::ApproveLogin(Options);
}

void ASMGameLiftSession::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void ASMGameLiftSession::NotifyLogout(const APlayerController* PC)
{
	Super::NotifyLogout(PC);
	UE_LOG(LogSMGameLiftSession, Log, TEXT("NotifyLogout"));
#if WITH_GAMELIFT
	FGameLiftServerSDKModule* GameLiftSDK = GetGameInstance()->GetSubsystem<UGameLift>()->GetSDK();
	if (!GameLiftSDK->GetGameSessionId().IsSuccess())
	{
		UE_LOG(LogSMGameLiftSession, Error, TEXT("GameLift GameSession is not abliable."))
		return;
	}
	const FString PlayerSessionId = PC->GetPlayerState<ASMPlayerState>()->GetGameLiftPlayerSessionId();
	if (PlayerSessionId.IsEmpty())
	{
		UE_LOG(LogSMGameLiftSession, Error,
		       TEXT("Failed to remove GameLift player session because PlayerSessionId is empty."));
		return;
	}
	GameLiftSDK->RemovePlayerSession(PlayerSessionId);
	UE_LOG(LogSMGameLiftSession, Log, TEXT("Player session removed: %s"), *PlayerSessionId);
#endif
}

void ASMGameLiftSession::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();
	UE_LOG(LogSMGameLiftSession, Log, TEXT("HandleMatchIsWaitingToStart"));

#if WITH_GAMELIFT
	UGameLift* GameLiftSystem = GetGameInstance()->GetSubsystem<UGameLift>();
	if (GameLiftSystem->IsInitialized())
	{
		GameLiftSystem->ProcessReady();
	}
#endif
}

void ASMGameLiftSession::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	UE_LOG(LogSMGameLiftSession, Log, TEXT("HandleMatchHasStarted"));
}

void ASMGameLiftSession::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
	UE_LOG(LogSMGameLiftSession, Log, TEXT("HandleMatchHasEnded"));

#if WITH_GAMELIFT
	FGameLiftServerSDKModule* GameLiftSDK = GetGameInstance()->GetSubsystem<UGameLift>()->GetSDK();
	FGameLiftGenericOutcome ProcessEndingOutcome = GameLiftSDK->ProcessEnding();
	if (!ProcessEndingOutcome.IsSuccess())
	{
		UE_LOG(LogSMGameLiftSession, Error, TEXT("Failed to process ending: [%s] %s"),
		       *ProcessEndingOutcome.GetError().m_errorName, *ProcessEndingOutcome.GetError().m_errorMessage);
		return;
	}

	UE_LOG(LogSMGameLiftSession, Log, TEXT("GameLift ProcessEnding succeeded."));
#endif
}
