// Copyright Surround, Inc. All Rights Reserved.


#include "SMViewModel.h"

#include "StereoMix.h"

USMViewModel::USMViewModel()
{
}

void USMViewModel::InitializeViewModel(UWorld* InWorld)
{
	UE_LOG(LogStereoMix, Verbose, TEXT("[ViewModel] Initializing %s"), *GetName())
}

void USMViewModel::BeginDestroy()
{
	Super::BeginDestroy();
	UE_LOG(LogStereoMix, Verbose, TEXT("[ViewModel] Destroying %s"), *GetName())
}
