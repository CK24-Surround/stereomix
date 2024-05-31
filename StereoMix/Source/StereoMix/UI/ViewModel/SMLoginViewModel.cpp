// Copyright Surround, Inc. All Rights Reserved.


#include "SMLoginViewModel.h"

USMLoginViewModel::USMLoginViewModel()
{
	UiState = ELoginUiState::Idle;
}

void USMLoginViewModel::InitializeViewModel(UWorld* InWorld)
{
	Super::InitializeViewModel(InWorld);
	
	if (!InWorld)
	{
		return;
	}

	AuthSubsystem = InWorld->GetGameInstance()->GetSubsystem<USMClientAuthSubsystem>();
}

void USMLoginViewModel::BeginDestroy()
{
	Super::BeginDestroy();
}

void USMLoginViewModel::SetUiState(const ELoginUiState NewState)
{
	UE_MVVM_SET_PROPERTY_VALUE(UiState, NewState);

	if (NewState == ELoginUiState::Submit)
	{
		OnSubmit();
	}
}

void USMLoginViewModel::OnSubmit()
{
}
