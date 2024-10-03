// Copyright Surround, Inc. All Rights Reserved.


#include "SMUserWidget_StartCountdown.h"

#include "Components/ScaleBox.h"
#include "Components/Core/SMRoundTimerComponent.h"
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
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	ASMGameState* GameState = World->GetGameState<ASMGameState>();
	if (GameState)
	{
		CachedTimerComponent = GameState->GetComponentByClass<USMRoundTimerComponent>();
		if (CachedTimerComponent.IsValid() && CachedTimerComponent->GetTimerState() == ESMTimerState::PreRound)
		{
			CachedTimerComponent->OnRemainingRoundTimeChanged.AddDynamic(this, &ThisClass::OnRemainingTimeChangedCallback);
			OnRemainingTimeChangedCallback(CachedTimerComponent->GetRemainingTime());
		}
	}
	else
	{
		World->GetTimerManager().SetTimerForNextTick(this, &ThisClass::BindGameState);
	}
}

void USMUserWidget_StartCountdown::OnRemainingTimeChangedCallback(int32 RemainingTime)
{
	if (RemainingTime == 3)
	{
		StartCountdown(RemainingTime);
	}

	if ((RemainingTime <= 3) && CachedTimerComponent.IsValid())
	{
		CachedTimerComponent->OnRemainingRoundTimeChanged.RemoveAll(this);
	}
}

void USMUserWidget_StartCountdown::StartCountdown(int32 RemainingTime)
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	const TWeakObjectPtr<USMUserWidget_StartCountdown> ThisWeakPtr = MakeWeakObjectPtr(this);
	auto ShowCountdown = [ThisWeakPtr](int32 CountdownNumber) {
		if (ThisWeakPtr.IsValid())
		{
			for (const TTuple<int, TWeakObjectPtr<UWidget>>& CountdownBox : ThisWeakPtr->CountdownBoxes)
			{
				if (UWidget* CountdownBoxWidget = CountdownBox.Value.Get())
				{
					CountdownBoxWidget->SetVisibility(ESlateVisibility::Hidden);
				}
			}

			TWeakObjectPtr<UWidget>* CountdownBoxPtr = ThisWeakPtr->CountdownBoxes.Find(CountdownNumber);
			UWidget* CountdownBoxWidget = CountdownBoxPtr ? CountdownBoxPtr->Get() : nullptr;
			if (CountdownBoxWidget)
			{
				CountdownBoxWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
		}
	};

	int32 CountdownInterval = 0;
	const AWorldSettings* WorldSettings = World->GetWorldSettings();
	const float OneSecond = WorldSettings ? WorldSettings->GetEffectiveTimeDilation() : 1.0f;
	for (int32 CountdownNumber = RemainingTime; CountdownNumber >= 0; --CountdownNumber)
	{
		if (CountdownInterval == 0)
		{
			ShowCountdown(CountdownNumber);
		}
		else
		{
			FTimerHandle ShowCountdownTimerHandle;
			World->GetTimerManager().SetTimer(ShowCountdownTimerHandle, [ShowCountdown, CountdownNumber]() { ShowCountdown(CountdownNumber); }, OneSecond * CountdownInterval, false);
		}

		++CountdownInterval;
	}

	FTimerHandle TimerHandle;
	World->GetTimerManager().SetTimer(TimerHandle, [ThisWeakPtr]() {
		if (ThisWeakPtr.IsValid())
		{
			ThisWeakPtr->SetVisibility(ESlateVisibility::Collapsed);
		}
	}, OneSecond * CountdownInterval, false);
}
