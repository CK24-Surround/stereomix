// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Data/SMTeam.h"
#include "Session/SMGameSession.h"

#include "SMRoomMode.generated.h"

class ASMRoomState;

/**
 * StereoMix Room Mode
 */
UCLASS()
class STEREOMIX_API ASMRoomMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASMRoomMode();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void InitGameState() override;

	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;

	bool IsReadyToStart() const;

	ASMRoomState* GetRoomState() const { return RoomState.Get(); }

	virtual void StartGame();

	virtual bool StartGameIfReadyToStart();

protected:
	UFUNCTION()
	void OnTeamPlayersUpdated(ESMTeam UpdatedTeam);

	UFUNCTION()
	void OnCountdownTick();

	UFUNCTION()
	void OnCountdownFinished();

	UFUNCTION()
	virtual void EmptyRoomCheckTick();

	UPROPERTY()
	TWeakObjectPtr<ASMGameSession> RoomSession;

private:
	UPROPERTY()
	TWeakObjectPtr<ASMRoomState> RoomState;
	FTimerHandle EmptyRoomCheckTimerHandle;
};
