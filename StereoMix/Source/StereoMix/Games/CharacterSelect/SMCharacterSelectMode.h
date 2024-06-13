// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SMCharacterSelectMode.generated.h"

class ASMCharacterSelectState;

/**
 * 
 */
UCLASS()
class STEREOMIX_API ASMCharacterSelectMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASMCharacterSelectMode();

	virtual void InitGameState() override;

	virtual void StartPlay() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

	UFUNCTION(BlueprintPure)
	ASMCharacterSelectState* GetCharacterSelectState() const { return CharacterSelectState.Get(); }

	UFUNCTION(BlueprintPure)
	bool IsAllPlayerReadyToSelect() const;

	UFUNCTION(BlueprintCallable)
	void ImmediateStart();

protected:
	UFUNCTION()
	void WaitingTimerTick();

	UFUNCTION()
	void OnCharacterSelectCountdownFinished();

	UFUNCTION()
	void StartGame();

private:
	UPROPERTY()
	TWeakObjectPtr<ASMCharacterSelectState> CharacterSelectState;

	UPROPERTY()
	FTimerHandle WaitingTimerHandle;

	UPROPERTY()
	int32 RemainingWaitingTime = 0;
};
