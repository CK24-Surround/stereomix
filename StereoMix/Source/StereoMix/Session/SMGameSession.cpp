// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameSession.h"

#include "StereoMix.h"
#include "Connection/SMGameServerConnectionSubsystem.h"

DEFINE_LOG_CATEGORY(LogSMGameSession)

// Sets default values
ASMGameSession::ASMGameSession(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	SessionName = TEXT("StereoMix GameSession");
	MaxPlayers = 6;
}

// Called when the game starts or when spawned
void ASMGameSession::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASMGameSession::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASMGameSession::InitOptions(const FString& Options)
{
	Super::InitOptions(Options);
	UE_LOG(LogSMGameSession, Verbose, TEXT("InitOptions: %s"), *Options)
}

void ASMGameSession::RegisterServer()
{
	Super::RegisterServer();
}

FString ASMGameSession::ApproveLogin(const FString& Options)
{
	UE_LOG(LogSMGameSession, Verbose, TEXT("ApproveLogin: %s"), *Options)
	return Super::ApproveLogin(Options);
}

void ASMGameSession::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	UE_LOG(LogSMGameSession, Verbose, TEXT("PostLogin"))
}

void ASMGameSession::RegisterPlayer(APlayerController* NewPlayer, const FUniqueNetIdRepl& UniqueId, bool bWasFromInvite)
{
	Super::RegisterPlayer(NewPlayer, UniqueId, bWasFromInvite);
	UE_LOG(LogSMGameSession, Verbose, TEXT("RegisterPlayer"))
}

void ASMGameSession::NotifyLogout(const APlayerController* PC)
{
	Super::NotifyLogout(PC);
	UE_LOG(LogSMGameSession, Verbose, TEXT("NotifyLogout"))
}

void ASMGameSession::UnregisterPlayer(const APlayerController* ExitingPlayer)
{
	Super::UnregisterPlayer(ExitingPlayer);
	UE_LOG(LogSMGameSession, Verbose, TEXT("UnregisterPlayer"))
}

void ASMGameSession::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();
	UE_LOG(LogSMGameSession, Verbose, TEXT("PostSeamlessTravel"))
}

bool ASMGameSession::HandleStartMatchRequest()
{
	UE_LOG(LogSMGameSession, Verbose, TEXT("HandleStartMatchRequest"))
	return Super::HandleStartMatchRequest();
}

void ASMGameSession::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();
	UE_LOG(LogSMGameSession, Verbose, TEXT("HandleMatchIsWaitingToStart"))
}

void ASMGameSession::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	UE_LOG(LogSMGameSession, Verbose, TEXT("HandleMatchHasStarted"))
}

void ASMGameSession::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
	UE_LOG(LogSMGameSession, Verbose, TEXT("HandleMatchHasEnded"))
}
