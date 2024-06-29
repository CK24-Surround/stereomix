// Copyright Surround, Inc. All Rights Reserved.


#include "SMViewModel.h"

USMViewModel::USMViewModel()
{
}

void USMViewModel::InitViewModel(APlayerController* PlayerController)
{
	OwningController = PlayerController;
}
