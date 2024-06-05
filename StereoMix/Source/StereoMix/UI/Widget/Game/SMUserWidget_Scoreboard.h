// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMUserWidget.h"
#include "Components/Image.h"
#include "Data/SMTeam.h"
#include "SMUserWidget_Scoreboard.generated.h"

class USMUserWidget_PhaseTimerBar;
class UTextBlock;
/**
 * 
 */
UCLASS()
class STEREOMIX_API USMUserWidget_Scoreboard : public USMUserWidget
{
	GENERATED_BODY()

public:
	USMUserWidget_Scoreboard();
	virtual bool Initialize() override;

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	virtual void SetASC(UAbilitySystemComponent* InASC) override;

protected:
	void BindToGameState();

	void OnChangeRoundTime(int32 RoundTime);

	void OnChangeEDMScore(int32 Score);

	void OnChangeFutureBaseScore(int32 Score);

	void OnChangePhaseTime(int32 RemainPhaseTime, int32 PhaseTime);

	void UpdatePhaseTime();

	void OnChangePhase(int32 CurrentPhaseNumber);

	void OnChangeEDMPhaseScore(int32 Score);
	
	void OnChangeFutureBassPhaseScore(int32 Score);

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> RoundTimer;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> EDMScore;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> FutureBassScore;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USMUserWidget_PhaseTimerBar> PhaseTimerBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PhaseNumber;

	TMap<ESMTeam, TArray<TWeakObjectPtr<UImage>>> PhaseScores;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> EDMPhaseScore1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> EDMPhaseScore2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> FutureBassPhase1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> FutureBassPhase2;

protected:
	float PhaseTimerBarRemainPhaseTime = 0.0f;

	float PhaseTimerBarPhaseTime = 0.0f;

	float PhaseTimerBarElapsedTime = 0.0f;
};
