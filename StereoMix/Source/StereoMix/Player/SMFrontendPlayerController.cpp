// Copyright Surround, Inc. All Rights Reserved.


#include "SMFrontendPlayerController.h"

ASMFrontendPlayerController::ASMFrontendPlayerController()
{
	bShowMouseCursor = true;
}

void ASMFrontendPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetInputMode(FInputModeUIOnly());
}
