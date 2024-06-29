// Copyright Surround, Inc. All Rights Reserved.


#include "SMFrontendViewModel.h"

#include "Controllers/SMFrontendPlayerController.h"
#include "StereoMixLog.h"

USMFrontendViewModel::USMFrontendViewModel()
{
	UiState = EFrontendUiState::Login;
	BackgroundColor = FLinearColor(FColor(90, 178, 255));
}

void USMFrontendViewModel::InitViewModel(APlayerController* PlayerController)
{
	Super::InitViewModel(PlayerController);

	FrontendPlayerController = CastChecked<ASMFrontendPlayerController>(PlayerController);
}

void USMFrontendViewModel::SetUiState(const EFrontendUiState NewUiState)
{
	if (!UE_MVVM_SET_PROPERTY_VALUE(UiState, NewUiState))
	{
		return;
	}

	UE_LOG(LogStereoMixUI, Verbose, TEXT("[SMFrontendViewModel] SetUiState: %s"), *UEnum::GetValueAsString(NewUiState));
}

void USMFrontendViewModel::SetBackgroundColor(const FLinearColor NewColor)
{
	UE_MVVM_SET_PROPERTY_VALUE(BackgroundColor, NewColor);
}

void USMFrontendViewModel::SetCurrentElementViewModel(USMFrontendElementViewModel* NewElementViewModel)
{
}
