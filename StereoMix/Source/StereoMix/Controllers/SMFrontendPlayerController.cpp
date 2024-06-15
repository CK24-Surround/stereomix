// Copyright Surround, Inc. All Rights Reserved.


#include "SMFrontendPlayerController.h"

ASMFrontendPlayerController::ASMFrontendPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Type::Default;
}

void ASMFrontendPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld()->GetGameViewport())
	{
		FrontendWidget = CreateWidget<USMFrontendWidget>(this, FrontendWidgetClass);
		FrontendWidget->AddToViewport();
	}
}
