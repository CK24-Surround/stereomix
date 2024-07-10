// Copyright Surround, Inc. All Rights Reserved.

#include "SMUserWidget_GameHUD.h"

#include "SMUserWidget_Scoreboard.h"

void USMUserWidget_GameHUD::SetASC(UAbilitySystemComponent* InASC)
{
	Super::SetASC(InASC);
	ScoreboardWidget->SetASC(InASC);
}

void USMUserWidget_GameHUD::NativeOnActivated()
{
	Super::NativeOnActivated();
}

UWidget* USMUserWidget_GameHUD::NativeGetDesiredFocusTarget() const
{
	return Super::NativeGetDesiredFocusTarget();
}

TOptional<FUIInputConfig> USMUserWidget_GameHUD::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::Game, EMouseCaptureMode::CapturePermanently_IncludingInitialMouseDown, EMouseLockMode::DoNotLock, false);
}
