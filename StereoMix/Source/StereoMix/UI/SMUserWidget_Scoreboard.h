// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SMUserWidget.h"
#include "Components/Image.h"
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

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> RoundTimer;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> EDMScore;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> FutureBaseScore;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USMUserWidget_PhaseTimerBar> PhaseTimerBar;

protected:
	float PhaseTimerBarRemainPhaseTime = 0.0f;

	float PhaseTimerBarPhaseTime = 0.0f;

	float PhaseTimerBarElapsedTime = 0.0f;
};
