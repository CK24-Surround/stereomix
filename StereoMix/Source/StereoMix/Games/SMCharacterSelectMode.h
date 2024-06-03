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

	UPROPERTY()
	TWeakObjectPtr<ASMCharacterSelectState> CharacterSelectState;

	
public:
	
	ASMCharacterSelectMode();

	virtual void InitGameState() override;

	virtual void StartPlay() override;

protected:
	UFUNCTION()
	void OnCountdownTick();

	UFUNCTION()
	void OnCountdownFinished();

	UFUNCTION()
	void StartGame();
};
