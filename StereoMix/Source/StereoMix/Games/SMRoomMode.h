// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGameStateNotify.h"
#include "GameFramework/GameModeBase.h"
#include "SMRoomMode.generated.h"

class ASMRoomState;

/**
 * StereoMix Room Mode
 */
UCLASS()
class STEREOMIX_API ASMRoomMode : public AGameModeBase
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<ASMRoomState> RoomState;

	
	
public:
	ASMRoomMode();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void InitGameState() override;

	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;

	virtual void StartPlay() override;

	bool IsReadyToStart() const;

protected:
	
	UFUNCTION()
	void OnTeamPlayersUpdated(ESMTeam UpdatedTeam);
	
	UFUNCTION()
	void OnCountdownTick();
	
	UFUNCTION()
	void OnCountdownFinished();
	
	virtual void StartGame();
};
