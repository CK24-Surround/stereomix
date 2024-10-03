// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/SMUserWidget.h"

#include "SMUserWidget_StartCountdown.generated.h"

class USMRoundTimerManagerComponent;
class ASMGameState;
class UScaleBox;
/**
 *
 */
UCLASS(Abstract)
class STEREOMIX_API USMUserWidget_StartCountdown : public USMUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	void BindGameState();

	/** 남은 시간이 변경될때마다 호출됩니다. */
	UFUNCTION()
	void OnRemainingTimeChangedCallback(int32 RemainingTime);

	void StartCountdown(int32 RemainingTime);

	/** 편하게 관리하기위해 맵으로 위젯들을 묶습니다. */
	UPROPERTY()
	TMap<int32, TWeakObjectPtr<UWidget>> CountdownBoxes;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScaleBox> Countdown3Box;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScaleBox> Countdown2Box;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScaleBox> Countdown1Box;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScaleBox> CountdownGoBox;

	TWeakObjectPtr<USMRoundTimerManagerComponent> CachedTimerComponent;
};
