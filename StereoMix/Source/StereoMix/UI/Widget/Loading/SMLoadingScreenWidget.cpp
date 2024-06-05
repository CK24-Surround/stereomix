// Copyright Surround, Inc. All Rights Reserved.


#include "SMLoadingScreenWidget.h"

#include "StereoMixLog.h"
#include "Animation/UMGSequencePlayer.h"

void USMLoadingScreenWidget::ShowLoadingScreen()
{
	SetRenderOpacity(1.0f);
}

void USMLoadingScreenWidget::HideLoadingScreen()
{
	if (UUMGSequencePlayer* Player = PlayAnimation(TransitionAnim, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f))
	{
		Player->OnSequenceFinishedPlaying().AddLambda([this](UUMGSequencePlayer&)
		{
			RemoveFromParent();
		});
	}
	else
	{
		RemoveFromParent();
	}
}

void USMLoadingScreenWidget::SetLoadingText(const FText& Text) const
{
	if (LoadingTextBlock)
	{
		LoadingTextBlock->SetText(Text);
	}
}
