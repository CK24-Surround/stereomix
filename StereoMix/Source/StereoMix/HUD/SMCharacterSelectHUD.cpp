// Copyright Surround, Inc. All Rights Reserved.


#include "SMCharacterSelectHUD.h"

#include "StereoMix.h"
#include "Games/SMCharacterSelectState.h"
#include "UI/ViewModel/SMViewModel_CharacterSelect.h"

ASMCharacterSelectHUD::ASMCharacterSelectHUD()
{
}

void ASMCharacterSelectHUD::BeginPlay()
{
	Super::BeginPlay();

	if (MainWidget)
	{
		if (USMViewModel_CharacterSelect* ViewModel = GetViewModelFromWidget<USMViewModel_CharacterSelect>(MainWidget, TEXT("SMViewModel_CharacterSelect")))
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
