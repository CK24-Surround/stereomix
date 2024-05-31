// Copyright Surround, Inc. All Rights Reserved.


#include "SMCharacterSelectHUD.h"

#include "StereoMix.h"
#include "Games/SMCharacterSelectState.h"
#include "UI/ViewModel/SMCharacterSelectionViewModel.h"

ASMCharacterSelectHUD::ASMCharacterSelectHUD()
{
}

void ASMCharacterSelectHUD::BeginPlay()
{
	Super::BeginPlay();

	if (MainWidget)
	{
		if (USMCharacterSelectionViewModel* ViewModel = GetViewModelFromWidget<USMCharacterSelectionViewModel>(MainWidget, TEXT("SMViewModel_CharacterSelect")))
		{
			CharacterSelectViewModel = ViewModel;
		}
	}

	if (!CharacterSelectViewModel)
	{
		UE_LOG(LogStereoMix, Error, TEXT("CharacterSelectViewModel is not set."))
		return;
	}
	
	CharacterSelectState = GetWorld()->GetGameState<ASMCharacterSelectState>();
	if (CharacterSelectState)
	{
		CharacterSelectViewModel->InitializeViewModel(GetWorld());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Current GameMode does not have a CharacterSelectState."))
	}
}
