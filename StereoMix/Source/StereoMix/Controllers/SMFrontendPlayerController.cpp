// Copyright Surround, Inc. All Rights Reserved.


#include "SMFrontendPlayerController.h"

#include "GameFramework/GameModeBase.h"
#include "GameInstance/SMGameInstance.h"

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
		USMGameInstance* GameInstance = GetGameInstance<USMGameInstance>();
		if (!GameInstance->IsViewedSplashScreen())
		{
			LocalTravel("/Game/StereoMix/Levels/Title/L_Title");
			return;
		}

		const FString& Options = GetWorld()->GetAuthGameMode()->OptionsString;
		FrontendWidget = CreateWidget<USMFrontendWidget>(this, FrontendWidgetClass);
		FrontendWidget->InitWidget(Options);
		FrontendWidget->AddToViewport();
	}
}
