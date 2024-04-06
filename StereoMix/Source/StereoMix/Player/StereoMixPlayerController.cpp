// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Data/StereoMixControlData.h"
#include "Utilities/StereoMixAssetPath.h"

AStereoMixPlayerController::AStereoMixPlayerController()
{
	static ConstructorHelpers::FObjectFinder<UStereoMixControlData> ControlDataRef(StereoMixAssetPath::ControlData);
	if (ControlDataRef.Object)
	{
		ControlData = ControlDataRef.Object;
	}
	else
	{
		UE_LOG(LogLoad, Error, TEXT("ControlData 로드에 실패했습니다."));
	}

	bShowMouseCursor = true;
}

void AStereoMixPlayerController::BeginPlay()
{
	Super::BeginPlay();

	InitControl();
}

void AStereoMixPlayerController::InitControl()
{
	if (!IsLocalController())
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->ClearAllMappings();
		Subsystem->AddMappingContext(ControlData->DefaultMappingContext, 0);
	}

	FInputModeGameOnly InputModeGameOnly;
	InputModeGameOnly.SetConsumeCaptureMouseDown(false);
	// SetInputMode(InputModeGameOnly);
}
