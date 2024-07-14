// Copyright Surround, Inc. All Rights Reserved.

#include "SMFrontendElementWidget.h"

#include "Animation/UMGSequencePlayer.h"
#include "FMODBlueprintStatics.h"
#include "SMFrontendWidget.h"
#include "StereoMixLog.h"

USMFrontendElementWidget::USMFrontendElementWidget()
{
	bAutoActivate = false;
	bSetVisibilityOnActivated = true;
	bSetVisibilityOnDeactivated = true;
	ActivatedVisibility = ESlateVisibility::Hidden;
	DeactivatedVisibility = ESlateVisibility::Collapsed;

	BackgroundColor = FLinearColor(FColor(169, 118, 236));
}

void USMFrontendElementWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BackButton)
	{
		BackButton->OnClicked().AddUObject(this, &USMFrontendElementWidget::HandleBackAction);
	}
}

void USMFrontendElementWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (bAutoTransitionOnActivate)
	{
		SetVisibility(ESlateVisibility::HitTestInvisible);
		UUMGSequencePlayer* TransitionIn = PlayTransitionIn();
		if (TransitionIn)
		{
			UE_LOG(LogStereoMixUI, Verbose, TEXT("[%s] NativeOnActivated - TransitionIn"), *GetName())
			TransitionIn->OnSequenceFinishedPlaying().AddWeakLambda(this, [this](UUMGSequencePlayer&) {
				UE_LOG(LogStereoMixUI, Verbose, TEXT("[%s] NativeOnActivated - TransitionIn Finished"), *GetName())
				SetVisibility(ESlateVisibility::Visible);
			});
		}
		else
		{
			UE_LOG(LogStereoMixUI, Verbose, TEXT("[%s] NativeOnActivated - No TransitionIn"), *GetName())
			SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
		UE_LOG(LogStereoMixUI, Verbose, TEXT("[%s] NativeOnActivated - No AutoTransitionOnActivate"), *GetName())
	}

	GetWorld()->GetTimerManager().SetTimerForNextTick([this] {
		GetParentFrontendWidget()->ChangeBackgroundColor(BackgroundColor);
	});

	if (TransitionInSound)
	{
		UFMODBlueprintStatics::PlayEvent2D(this, TransitionInSound, true);
	}
}

void USMFrontendElementWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
	if (TransitionOutSound)
	{
		UFMODBlueprintStatics::PlayEvent2D(this, TransitionOutSound, true);
	}
	GetParentFrontendWidget()->ClearPopups();
}

void USMFrontendElementWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (BackButton)
	{
		BackButton->OnClicked().Clear();
	}
}

UUMGSequencePlayer* USMFrontendElementWidget::PlayTransitionIn()
{
	if (TransitionAnim)
	{
		UUMGSequencePlayer* Player = PlayAnimationForward(TransitionAnim);
		Player->OnSequenceFinishedPlaying().AddWeakLambda(this, [&](UUMGSequencePlayer&) {
			bTransitioning = false;
		});
		bTransitioning = true;
		return Player;
	}

	return nullptr;
}

UUMGSequencePlayer* USMFrontendElementWidget::PlayTransitionOut()
{
	if (TransitionAnim)
	{
		UUMGSequencePlayer* Player = PlayAnimationReverse(TransitionAnim);
		Player->OnSequenceFinishedPlaying().AddWeakLambda(this, [&](UUMGSequencePlayer&) {
			bTransitioning = false;
		});
		bTransitioning = true;
		return Player;
	}
	return nullptr;
}

bool USMFrontendElementWidget::NativeOnHandleBackAction()
{
	if (IsTransitioning())
	{
		return false;
	}

	if (bIsBackHandler)
	{
		GetParentFrontendWidget()->RemoveElementWidget(this);
		return true;
	}
	return false;
}

TOptional<FUIInputConfig> USMFrontendElementWidget::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture, EMouseLockMode::DoNotLock);
}
