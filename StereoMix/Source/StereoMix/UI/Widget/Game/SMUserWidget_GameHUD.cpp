// Copyright Surround, Inc. All Rights Reserved.


#include "SMUserWidget_GameHUD.h"

#include "SMUserWidget_Scoreboard.h"
#include "StereoMixLog.h"

void USMUserWidget_GameHUD::SetASC(UAbilitySystemComponent* InASC)
{
	Super::SetASC(InASC);

	ScoreboardWidget->SetASC(InASC);
}

void USMUserWidget_GameHUD::NativeOnActivated()
{
	Super::NativeOnActivated();
	UE_LOG(LogStereoMixUI, Verbose, TEXT("USMUserWidget_GameHUD::NativeOnActivated"))
}

TOptional<FUIInputConfig> USMUserWidget_GameHUD::GetDesiredInputConfig() const
{
	UE_LOG(LogStereoMixUI, Verbose, TEXT("USMUserWidget_GameHUD::GetDesiredInputConfig"))

	// TODO: 원래는 캡처가 안되어야 하는데, 캡처가 계속 되는 문제가 있어서 임시로 캡처 시 커서가 보이게 변경함. 추후 수정 필요

	const FUIInputConfig InputConfig = FUIInputConfig(
		ECommonInputMode::Game,
		EMouseCaptureMode::CapturePermanently_IncludingInitialMouseDown,
		EMouseLockMode::DoNotLock,
		false);

	return TOptional(InputConfig);
}
