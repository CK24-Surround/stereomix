// Copyright Surround, Inc. All Rights Reserved.


#include "SMMainMenuWidget.h"

#include "SMFrontendWidget.h"
#include "Kismet/GameplayStatics.h"

USMMainMenuWidget::USMMainMenuWidget()
{
	bIsBackHandler = false;
}

void USMMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	LobbyButton->OnClicked().AddUObject(this, &USMMainMenuWidget::OnLobbyButtonClicked);
	TutorialButton->OnClicked().AddUObject(this, &USMMainMenuWidget::OnTutorialButtonClicked);
	SettingsButton->OnClicked().AddUObject(this, &USMMainMenuWidget::OnSettingsButtonClicked);
	QuitButton->OnClicked().AddUObject(this, &USMMainMenuWidget::OnQuitButtonClicked);
}

void USMMainMenuWidget::NativeDestruct()
{
	Super::NativeDestruct();

	LobbyButton->OnClicked().Clear();
	TutorialButton->OnClicked().Clear();
	SettingsButton->OnClicked().Clear();
	QuitButton->OnClicked().Clear();
}

void USMMainMenuWidget::OnLobbyButtonClicked()
{
	GetParentFrontendWidget()->AddElementWidget(LobbyWidgetClass);
}

void USMMainMenuWidget::OnTutorialButtonClicked()
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, TutorialLevel);
}

void USMMainMenuWidget::OnSettingsButtonClicked()
{
	GetParentFrontendWidget()->AddElementWidget(SettingsWidgetClass);
}

void USMMainMenuWidget::OnQuitButtonClicked()
{
	LobbyButton->SetIsEnabled(false);
	TutorialButton->SetIsEnabled(false);
	SettingsButton->SetIsEnabled(false);
	QuitButton->SetIsEnabled(false);

	GetParentFrontendWidget()->ChangeBackgroundColor(FLinearColor(FColor(151, 36, 36)));
	PlayTransitionOut();

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]
	{
		UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
	}, 1.0f, false);
}
