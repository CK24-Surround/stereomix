// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SMRoomMode.generated.h"

/**
 * StereoMix Room Mode
 */
UCLASS()
class STEREOMIX_API ASMRoomMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASMRoomMode();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void InitGameState() override;

	bool IsReadyToStart();

protected:
	UPROPERTY(EditAnywhere, Category = "Room Config")
	int32 CountdownTime;
	
	virtual void OnPostLogin(AController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

	void UpdateRoomState();

	virtual void CountdownTick();

	virtual void StartGame();

private:
	FTimerHandle CountdownTimerHandle;
};
