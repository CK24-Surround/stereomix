// Copyright Surround, Inc. All Rights Reserved.


#include "SMMainMenuHUD.h"

#include "Connection/SMClientConnectionSubsystem.h"
#include "UI/ViewModel/SMMainMenuViewModel.h"

ASMMainMenuHUD::ASMMainMenuHUD()
{
}

void ASMMainMenuHUD::BeginPlay()
{
	Super::BeginPlay();

	if (MainWidget)
	{
		if (auto ViewModel = GetViewModelFromWidget<USMMainMenuViewModel>(MainWidget, TEXT("SMViewModel_MainMenu")))
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
