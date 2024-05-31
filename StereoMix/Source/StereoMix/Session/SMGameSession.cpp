// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameSession.h"

#include "StereoMix.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogSMGameSession)

// Sets default values
ASMGameSession::ASMGameSession()
{
	PrimaryActorTick.bCanEverTick = true;
	SessionName = TEXT("StereoMix GameSession");
	MaxPlayers = 6;

#if WITH_EDITOR
	bCanEnterRoom = true;
#else
	bCanEnterRoom = false;
#endif
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
	// BUG: MaxPlayers가 생성자가 아니라 실제 여기서 적용됨 
	MaxPlayers = 6;
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

	if (!bCanEnterRoom)
	{
		return TEXT("Room is closed.");
	}
	
	return Super::ApproveLogin(Options);
}

void ASMGameSession::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
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
