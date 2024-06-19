// Copyright Surround, Inc. All Rights Reserved.


#include "SMFrontendPlayerController.h"

#include "GameFramework/GameModeBase.h"

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
		const FString& Options = GetWorld()->GetAuthGameMode()->OptionsString;
		FrontendWidget = CreateWidget<USMFrontendWidget>(this, FrontendWidgetClass);
		FrontendWidget->InitWidget(Options);
		FrontendWidget->AddToViewport();
	}
}