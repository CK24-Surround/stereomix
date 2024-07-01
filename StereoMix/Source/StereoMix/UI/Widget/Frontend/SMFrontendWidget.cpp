// Copyright Surround, Inc. All Rights Reserved.

#include "SMFrontendWidget.h"

#include "GameFramework/GameModeBase.h"
#include "Animation/UMGSequencePlayer.h"
#include "Backend/Client/SMClientAuthSubsystem.h"
#include "GameInstance/SMGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "SMFrontendElementWidget.h"
#include "StereoMixLog.h"

void FFrontendBackgroundColorState::ChangeBackgroundColor(const FLinearColor NewColor)
{
	bIsTransitioning = true;
	NextColor = NewColor;
}

FLinearColor FFrontendBackgroundColorState::TransitionBackgroundColorOnTick(const float DeltaTime)
{
	if (bIsTransitioning)
	{
		if (CurrentColor == NextColor)
		{
			bIsTransitioning = false;
		}
		else
		{
			CurrentColor.R = FMath::FInterpConstantTo(CurrentColor.R, NextColor.R, DeltaTime, TransitionSpeed);
			CurrentColor.G = FMath::FInterpConstantTo(CurrentColor.G, NextColor.G, DeltaTime, TransitionSpeed);
			CurrentColor.B = FMath::FInterpConstantTo(CurrentColor.B, NextColor.B, DeltaTime, TransitionSpeed);
			CurrentColor.A = FMath::FInterpConstantTo(CurrentColor.A, NextColor.A, DeltaTime, TransitionSpeed);
			bIsTransitioning = true;
		}
	}

	return CurrentColor;
}

USMFrontendWidget::USMFrontendWidget()
{
}

void USMFrontendWidget::InitWidget(const FString& LevelOptions)
{
	UE_LOG(LogStereoMixUI, Verbose, TEXT("[%s] InitWidget - LevelOptions: %s"), *GetName(), *LevelOptions)

	if (FString WidgetOverride; FParse::Value(*LevelOptions, TEXT("init="), WidgetOverride))
	{
		if (WidgetOverride == TEXT("login"))
		{
			InitWidgetClass = LoginWidgetClass;
			return;
		}
		if (WidgetOverride == TEXT("mainmenu"))
		{
			InitWidgetClass = MainMenuWidgetClass;
			return;
		}

		UE_LOG(LogStereoMixUI, Warning, TEXT("[%s] InitWidget - Invalid Widget Override: %s"), *GetName(), *WidgetOverride)
	}

	const USMGameInstance& GameInstance = GetGameInstanceChecked<USMGameInstance>();
	if (GameInstance.IsDemoGame())
	{
		UE_LOG(LogStereoMixUI, Verbose, TEXT("[%s] InitWidget - Demo Mode"), *GetName())
		if (USMClientAuthSubsystem* AuthSubsystem = GetGameInstance()->GetSubsystem<USMClientAuthSubsystem>())
		{
			AuthSubsystem->ResetAccount();
		}
		InitWidgetClass = LoginWidgetClass;
	}
	else if (GameInstance.IsCustomGame())
	{
		UE_LOG(LogStereoMixUI, Verbose, TEXT("[%s] InitWidget - CustomServer Mode"), *GetName())
		InitWidgetClass = CustomServerWidgetClass;
	}
	else
	{
		if (GetGameInstance()->GetSubsystem<USMClientAuthSubsystem>()->IsAuthenticated())
		{
			UE_LOG(LogStereoMixUI, Verbose, TEXT("[%s] InitWidget - MainMenu"), *GetName())
			InitWidgetClass = MainMenuWidgetClass;
		}
		else
		{
			UE_LOG(LogStereoMixUI, Verbose, TEXT("[%s] InitWidget - Login (Not Authenticated)"), *GetName())
			InitWidgetClass = LoginWidgetClass;
		}
	}
}

void USMFrontendWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void USMFrontendWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BackgroundColorState = FFrontendBackgroundColorState();
	BackgroundColorState.bIsTransitioning = false;
	BackgroundColorState.TransitionSpeed = BackgroundTransitionSpeed;
	if (Background)
	{
		BackgroundColorState.CurrentColor = Background->GetBrushColor();
		BackgroundColorState.NextColor = Background->GetBrushColor();
	}

	if (ensure(InitWidgetClass))
	{
		AddElementWidget(InitWidgetClass);
	}
}

void USMFrontendWidget::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (Background && BackgroundColorState.bIsTransitioning)
	{
		Background->SetBrushColor(BackgroundColorState.TransitionBackgroundColorOnTick(InDeltaTime));
	}
}

USMFrontendElementWidget* USMFrontendWidget::AddElementWidgetInternal(const TSubclassOf<USMFrontendElementWidget>& WidgetClass)
{
	USMFrontendElementWidget* NewWidget = MainStack->AddWidget<USMFrontendElementWidget>(WidgetClass, [&](USMFrontendElementWidget& AddedWidget) { AddedWidget.ParentFrontendWidget = this; });
	UE_LOG(LogStereoMixUI, Verbose, TEXT("[%s] AddElementWidgetInternal - NewWidget: %s"), *GetName(), *NewWidget->GetName())
	NewWidget->ActivateWidget();
	return NewWidget;
}

void USMFrontendWidget::RemoveElementWidgetInternal(USMFrontendElementWidget* Widget)
{
	MainStack->RemoveWidget(*Widget);
}

void USMFrontendWidget::AddElementWidget(const TSubclassOf<USMFrontendElementWidget> WidgetClass)
{
	AddElementWidget(WidgetClass, [](USMFrontendElementWidget&) {});
}

void USMFrontendWidget::AddElementWidget(TSubclassOf<USMFrontendElementWidget> WidgetClass, TFunctionRef<void(USMFrontendElementWidget&)> InstanceInitFunc)
{
	UCommonActivatableWidget* CurrentActiveWidget = MainStack->GetActiveWidget();
	if (!CurrentActiveWidget)
	{
		UE_LOG(LogStereoMixUI, Verbose, TEXT("[%s] AddElementWidget - No Active Widget"), *GetName())
		USMFrontendElementWidget* NewWidget = AddElementWidgetInternal(WidgetClass);
		InstanceInitFunc(*NewWidget);
		return;
	}

	// 기존 위젯 트랜지션 아웃 애니메이션 재생
	USMFrontendElementWidget* CurrentActiveElementWidget = CastChecked<USMFrontendElementWidget>(CurrentActiveWidget);
	if (UUMGSequencePlayer* TransitionOut = CurrentActiveElementWidget->PlayTransitionOut())
	{
		UE_LOG(LogStereoMixUI, Verbose, TEXT("[%s] AddElementWidget - TransitionOut"), *GetName())
		TransitionOut->OnSequenceFinishedPlaying().AddWeakLambda(this, [&, WidgetClass, InstanceInitFunc](UUMGSequencePlayer&) {
			USMFrontendElementWidget* NewWidget = AddElementWidgetInternal(WidgetClass);
			InstanceInitFunc(*NewWidget);
		});
	}
	else
	{
		UE_LOG(LogStereoMixUI, Verbose, TEXT("[%s] AddElementWidget - No TransitionOut"), *GetName())
		USMFrontendElementWidget* NewWidget = AddElementWidgetInternal(WidgetClass);
		InstanceInitFunc(*NewWidget);
	}
}

void USMFrontendWidget::AddElementWidgetInstance(USMFrontendElementWidget& WidgetInstance)
{
	auto AddWidgetFunc = [this, &WidgetInstance] {
		WidgetInstance.ParentFrontendWidget = this;
		MainStack->AddWidgetInstance(WidgetInstance);
		UE_LOG(LogStereoMixUI, Verbose, TEXT("[%s] AddElementWidgetInstance - NewWidget: %s"), *GetName(), *WidgetInstance.GetName())
		WidgetInstance.ActivateWidget();
	};

	// 기존 위젯 트랜지션 아웃 애니메이션 재생
	UCommonActivatableWidget* CurrentActiveWidget = MainStack->GetActiveWidget();
	if (!CurrentActiveWidget)
	{
		UE_LOG(LogStereoMixUI, Verbose, TEXT("[%s] AddElementWidgetInstance - No Active Widget"), *GetName())
		AddWidgetFunc();
		return;
	}

	USMFrontendElementWidget* CurrentActiveElementWidget = CastChecked<USMFrontendElementWidget>(CurrentActiveWidget);
	UUMGSequencePlayer* TransitionOut = CurrentActiveElementWidget->PlayTransitionOut();
	if (TransitionOut)
	{
		UE_LOG(LogStereoMixUI, Verbose, TEXT("[%s] AddElementWidgetInstance - TransitionOut"), *GetName())
		TransitionOut->OnSequenceFinishedPlaying().AddWeakLambda(this, [AddWidgetFunc](UUMGSequencePlayer&) { AddWidgetFunc(); });
	}
	else
	{
		UE_LOG(LogStereoMixUI, Verbose, TEXT("[%s] AddElementWidgetInstance - No TransitionOut"), *GetName())
		AddWidgetFunc();
	}
}

void USMFrontendWidget::RemoveElementWidget(USMFrontendElementWidget* Widget)
{
	if (!Widget)
	{
		return;
	}

	if (Widget == MainStack->GetActiveWidget())
	{
		UE_LOG(LogStereoMixUI, Verbose, TEXT("[%s] RemoveElementWidget - Active Widget: %s"), *GetName(), *Widget->GetName())
		Widget->SetVisibility(ESlateVisibility::HitTestInvisible);
		UUMGSequencePlayer* TransitionOut = Widget->PlayTransitionOut();
		if (TransitionOut)
		{
			UE_LOG(LogStereoMixUI, Verbose, TEXT("[%s] RemoveElementWidget - TransitionOut"), *GetName())
			TransitionOut->OnSequenceFinishedPlaying().AddWeakLambda(this, [this, Widget](UUMGSequencePlayer&) {
				UE_LOG(LogStereoMixUI, Verbose, TEXT("[%s] RemoveElementWidget - TransitionOut Finished"), *GetName())
				RemoveElementWidgetInternal(Widget);
			});
		}
		else
		{
			UE_LOG(LogStereoMixUI, Verbose, TEXT("[%s] RemoveElementWidget - No TransitionOut"), *GetName())
			RemoveElementWidgetInternal(Widget);
		}
	}
	else
	{
		UE_LOG(LogStereoMixUI, Verbose, TEXT("[%s] RemoveElementWidget - Inactive Widget: %s"), *GetName(), *Widget->GetName())
		RemoveElementWidgetInternal(Widget);
	}
}

USMPopup* USMFrontendWidget::AddPopup(const TSubclassOf<USMPopup> PopupClass)
{
	if (PopupClass->IsChildOf<USMActivatableWidget>())
	{
		UE_LOG(LogStereoMixUI, Verbose, TEXT("[%s] AddPopup: %s"), *GetName(), *PopupClass->GetName())
		USMPopup* Popup = Cast<USMPopup>(PopupStack->AddWidget(PopupClass));
		Popup->ActivateWidget();
		Popup->OnSubmit.Unbind();
		Popup->OnClose.Unbind();
		Popup->OnDeactivated().RemoveAll(this);
		Popup->OnDeactivated().AddWeakLambda(this, [this] { SetIsFocusable(true); });
		SetIsFocusable(false);

		return Popup;
	}
	return nullptr;
}

USMAlertPopup* USMFrontendWidget::ShowAlert(const FString& AlertText)
{
	if (USMPopup* Popup = AddPopup(AlertPopupClass))
	{
		UE_LOG(LogStereoMixUI, Verbose, TEXT("[%s] ShowAlert: %s"), *GetName(), *AlertText)
		USMAlertPopup* AlertPopup = Cast<USMAlertPopup>(Popup);
		AlertPopup->SetAlertText(FText::FromString(AlertText));
		return AlertPopup;
	}
	return nullptr;
}

void USMFrontendWidget::RemoveTopElementWidget()
{
	RemoveElementWidget(CastChecked<USMFrontendElementWidget>(MainStack->GetActiveWidget()));
}

void USMFrontendWidget::ChangeBackgroundColor(const FLinearColor NewColor)
{
	BackgroundColorState.TransitionSpeed = BackgroundTransitionSpeed;
	BackgroundColorState.CurrentColor = Background->GetBrushColor();
	UE_LOG(LogStereoMixUI, Verbose, TEXT("[%s] ChangeBackgroundColor: %s->%s"), *GetName(), *BackgroundColorState.CurrentColor.ToString(), *NewColor.ToString());
	BackgroundColorState.ChangeBackgroundColor(NewColor);
}

void USMFrontendWidget::OnTransitionInFinished(UUMGSequencePlayer& SequencePlayer) 
{
}