// Fill out your copyright notice in the Description page of Project Settings.


#include "Games/SMGameSession.h"

#include "GameLiftServerSDK.h"
#include "Kismet/GameplayStatics.h"

#if WITH_GAMELIFT
#include "API/GameLift.h"
#endif

ASMGameSession::ASMGameSession()
{
}

FString ASMGameSession::ApproveLogin(const FString& Options)
{
	UE_LOG(LogSMGameSession, Log, TEXT("ApproveLogin: %s"), *Options);

#if WITH_GAMELIFT
	FGameLiftServerSDKModule* GameLiftSDK = GetGameInstance()->GetSubsystem<UGameLift>()->GetSDK();

	const FGameLiftStringOutcome GetGameSessionIdOutcome = GameLiftSDK->GetGameSessionId();
	// Check GameLift GameSession is initialized


	// Options="PlayerSessionId=[PlayerSessionId]"
	const FString PlayerSessionId = UGameplayStatics::ParseOption(Options, TEXT("PlayerSessionId"));
	if (PlayerSessionId.IsEmpty())
	{
		return TEXT("New player has no PlayerSessionId.");
	}

	UE_LOG(LogSMGameSession, Log, TEXT("Accepting new GameLift player session: %s"), *PlayerSessionId);


	// GameLift AcceptPlayerSession Failed
	if (const FGameLiftGenericOutcome AcceptPlayerSessionOutcome
		= GameLiftSDK->AcceptPlayerSession(Options); !AcceptPlayerSessionOutcome.IsSuccess())
	{
		UE_LOG(LogSMGameSession, Error,
		       TEXT("Failed to accept player session: %s"), *AcceptPlayerSessionOutcome.GetError().m_errorMessage);

		return TEXT("GameLift AcceptPlayerSession failed.");
	}
#else
	UE_LOG(LogSMGameSession, Warning, TEXT("GameLift is not enabled. Login will always succeed."));
#endif

	return Super::ApproveLogin(Options);
}

void ASMGameSession::OnStartSessionComplete(FName InSessionName, bool bWasSuccessful)
{
	Super::OnStartSessionComplete(InSessionName, bWasSuccessful);
	UE_LOG(LogSMGameSession, Log, TEXT("OnStartSessionComplete: %s"), *InSessionName.ToString());
}

void ASMGameSession::OnEndSessionComplete(FName InSessionName, bool bWasSuccessful)
{
	Super::OnEndSessionComplete(InSessionName, bWasSuccessful);
	UE_LOG(LogSMGameSession, Log, TEXT("OnEndSessionComplete: %s"), *InSessionName.ToString());
}
