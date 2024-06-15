// Copyright Surround, Inc. All Rights Reserved.


#include "SMTutorialPlayerController.h"

ASMTutorialPlayerController::ASMTutorialPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Type::Default;
}

void ASMTutorialPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld()->GetGameViewport())
	{
		TutorialWidget = CreateWidget<USMTutorialWidget>(this, TutorialWidgetClass);
		TutorialWidget->AddToViewport();
	}
}
