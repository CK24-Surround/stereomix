// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "SMGameLiftSession.generated.h"

DECLARE_LOG_CATEGORY_CLASS(LogSMGameLiftSession, Log, All);

UCLASS()
class STEREOMIX_API ASMGameLiftSession : public AGameSession
{
	GENERATED_BODY()

public:
	ASMGameLiftSession();

	virtual void BeginPlay() override;

	// Register GameLift
	virtual void RegisterServer() override;
	virtual void RegisterServerFailed() override;

#if WITH_GAMELIFT
	virtual void OnTerminateFromGameLift();
#endif

	// Login
	virtual FString ApproveLogin(const FString& Options) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// Logout
	virtual void NotifyLogout(const APlayerController* PC) override;

	// Match
	virtual void HandleMatchIsWaitingToStart() override;
	virtual void HandleMatchHasStarted() override;
	virtual void HandleMatchHasEnded() override;
};
