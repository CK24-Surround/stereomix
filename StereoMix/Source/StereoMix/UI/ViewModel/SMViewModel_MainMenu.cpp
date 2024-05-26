// Copyright Surround, Inc. All Rights Reserved.


#include "SMViewModel_MainMenu.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void USMViewModel_MainMenu::StartGame()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("L_Lobby"));
}

void USMViewModel_MainMenu::StartTutorial()
{
	// UGameplayStatics::OpenLevel(GetWorld(), TEXT("L_Tutorial"));
}

void USMViewModel_MainMenu::ShowSettings()
{
	// check(SettingsWidgetClass)
	// if (!SettingsWidget)
	// {
	// 	SettingsWidget = CreateWidget<UUserWidget>(GetWorld(), SettingsWidgetClass);
	// }
	// SettingsWidget->AddToViewport(5);
}

void USMViewModel_MainMenu::QuitGame()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, true);
}
