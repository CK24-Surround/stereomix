// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMUserWidget.h"
#include "SMUserWidget_StartCountdown.generated.h"

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

	void BindGameState();

	/** 카운트 다운이 변경될때마다 호출됩니다. */
	void OnCountdownTimeChangeCallback(int32 CountdownNumber);

	/** 요청한 숫자로 카운트 다운 위젯을 출력해줍니다.*/
	void ChangeCountdownNumber(int32 CountdownNumber);

	/** 카운트다운을 비활성화합니다. */
	void DisableCountdown();

	TWeakObjectPtr<ASMGameState> CachedGameState;
};
