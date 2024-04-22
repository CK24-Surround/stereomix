// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "SMGameSession.generated.h"

DECLARE_LOG_CATEGORY_CLASS(LogSMGameSession, Log, All);

UCLASS()
class STEREOMIX_API ASMGameSession : public AGameSession
{
	GENERATED_BODY()

public:
	ASMGameSession();

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual FString ApproveLogin(const FString& Options) override;

	virtual void UnregisterPlayer(const APlayerController* ExitingPlayer) override;

	virtual bool HandleStartMatchRequest() override;

	virtual void HandleMatchHasEnded() override;

	virtual void HandleMatchHasStarted() override;

	virtual void RegisterServer() override;
};
