// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "SMGameMode.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API ASMGameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	/** 플레이어 입장시 전송된 닉네임으로 플레이어 스테이트를 초기화해줍니다. */
	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;

protected:
	virtual void PostInitializeComponents() override;

protected:
	virtual void EndMatch() override;

protected:
	void BindToGameState();

	void OnEndRoundTimer();

	void OnEndVictoryDefeatTimer();
};
