// Copyright Surround, Inc. All Rights Reserved.


#include "SMMainMenuHUD.h"

#include "Connection/SMClientConnectionSubsystem.h"
#include "UI/ViewModel/SMViewModel_MainMenu.h"

ASMMainMenuHUD::ASMMainMenuHUD()
{
}

void ASMMainMenuHUD::BeginPlay()
{
	Super::BeginPlay();

	if (MainWidget)
	{
		if (auto ViewModel = GetViewModelFromWidget<USMViewModel_MainMenu>(MainWidget, TEXT("SMViewModel_MainMenu")))
		{
			MainMenuViewModel = ViewModel;
		}
	}
	
	if (const auto ConnectionSubsystem = GetGameInstance()->GetSubsystem<USMClientConnectionSubsystem>())
	{
		MainMenuViewModel->SetUserNameText(ConnectionSubsystem->GetAccount().UserName);
	}
}

void ASMMainMenuHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
