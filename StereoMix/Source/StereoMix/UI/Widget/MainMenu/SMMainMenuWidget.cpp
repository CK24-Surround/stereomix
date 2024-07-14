// Copyright Surround, Inc. All Rights Reserved.


#include "SMMainMenuWidget.h"

#include "CommonTextBlock.h"
#include "GameInstance/SMGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "StereoMixLog.h"
#include "Backend/Client/SMClientAuthSubsystem.h"
#include "UI/Widget/Frontend/SMFrontendWidget.h"
#include "UI/Widget/Lobby/SMLobbyWidget.h"
#include "UI/Widget/Settings/SMSettingsWidget.h"

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

	if (USMClientAuthSubsystem* AuthSubsystem = GetGameInstance()->GetSubsystem<USMClientAuthSubsystem>(); AuthSubsystem && AuthSubsystem->IsAuthenticated())
	{
		UserIdTextBlock->SetText(FText::FromString(AuthSubsystem->GetUserAccount()->GetUserId()));
		UserNameTextBlock->SetText(FText::FromString(AuthSubsystem->GetUserAccount()->GetUserName()));
	}
	else
	{
		UE_LOG(LogStereoMixUI, Warning, TEXT("[SMMainMenuWidget] Failed to get user account"))
		UserIdTextBlock->SetText(FText::FromString(TEXT("Unknown")));
		UserNameTextBlock->SetText(FText::FromString(TEXT("Unknown")));
	}
	GameVersionTextBlock->SetText(FText::FromString(USMGameInstance::GetGameVersion()));
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
	GetParentFrontendWidget()->AddElementWidget(LobbyWidgetClass.Get());
}

void USMMainMenuWidget::OnTutorialButtonClicked()
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, TutorialLevel);
}

void USMMainMenuWidget::OnSettingsButtonClicked()
{
	GetParentFrontendWidget()->AddElementWidget(SettingsWidgetClass.Get());
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
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&] { UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false); }, 1.0f, false);
}
