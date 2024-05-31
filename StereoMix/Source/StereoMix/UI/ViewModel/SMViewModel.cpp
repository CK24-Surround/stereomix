// Copyright Surround, Inc. All Rights Reserved.


#include "SMViewModel.h"

#include "MVVMSubsystem.h"
#include "StereoMix.h"
#include "GameFramework/GameStateBase.h"

USMViewModel::USMViewModel()
{
}

void USMViewModel::InitializeViewModel(UWorld* InWorld)
{
	UE_LOG(LogStereoMix, Verbose, TEXT("[ViewModel] Initializing %s"), *GetFName().ToString())

	if (InWorld)
	{
		OwningGameState = InWorld->GetGameState();
		OwningPlayerController = InWorld->GetFirstPlayerController();
	}
	UMVVMSubsystem::GetViewFromUserWidget()
}

void USMViewModel::BeginDestroy()
{
	Super::BeginDestroy();
	UE_LOG(LogStereoMix, Verbose, TEXT("[ViewModel] Destroying %s"), *GetFName().ToString())
}
