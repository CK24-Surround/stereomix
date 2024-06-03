// Copyright Surround, Inc. All Rights Reserved.


#include "SMUserWidget_StartCountdown.h"

#include "Components/ScaleBox.h"
#include "Games/SMGameState.h"
#include "Utilities/SMLog.h"

void USMUserWidget_StartCountdown::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	CountdownBoxes.Add(3, Countdown3Box);
	CountdownBoxes.Add(2, Countdown2Box);
	CountdownBoxes.Add(1, Countdown1Box);
	CountdownBoxes.Add(0, CountdownGoBox);

	BindGameState();
}

void USMUserWidget_StartCountdown::BindGameState()
{
	CachedGameState = GetWorld()->GetGameState<ASMGameState>();
	if (CachedGameState.Get())
	{
		CachedGameState->OnChangeCountdownTime.BindUObject(this, &ThisClass::OnCountdownTimeChangeCallback);
	}
	else
	{
		// 만약 바인드에 실패하면 계속 시도합니다.`
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::BindGameState);
	}
}

void USMUserWidget_StartCountdown::OnCountdownTimeChangeCallback(int32 CountdownNumber)
{
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	ChangeCountdownNumber(CountdownNumber);
}

void USMUserWidget_StartCountdown::ChangeCountdownNumber(int32 CountdownNumber)
{
	for (const auto& CountdownBox : CountdownBoxes)
	{
		if (ensureAlways(CountdownBox.Value.Get()))
		{
			CountdownBox.Value->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (ensureAlways(CountdownBoxes.Find(CountdownNumber)))
	{
		CountdownBoxes[CountdownNumber]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	if (CountdownNumber == 0)
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::DisableCountdown, 1.0f);
	}
}

void USMUserWidget_StartCountdown::DisableCountdown()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
