// Copyright Surround, Inc. All Rights Reserved.


#include "SMLoginViewModel.h"

#include "Controllers/SMFrontendPlayerController.h"
#include "SMFrontendViewModel.h"

USMLoginViewModel::USMLoginViewModel()
{
	UiState = ELoginViewModelUiState::Idle;
}

void USMLoginViewModel::BeginDestroy()
{
	Super::BeginDestroy();
}

void USMLoginViewModel::SetUiState(const ELoginViewModelUiState NewState)
{
	UE_MVVM_SET_PROPERTY_VALUE(UiState, NewState);

	if (NewState == ELoginViewModelUiState::Submit)
	{
		OnSubmit();
	}
}

void USMLoginViewModel::OnSubmit()
{
}
