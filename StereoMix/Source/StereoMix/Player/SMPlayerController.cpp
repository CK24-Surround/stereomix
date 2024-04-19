// Fill out your copyright notice in the Description page of Project Settings.


#include "SMPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "SMPlayerState.h"
#include "Blueprint/UserWidget.h"
#include "Data/SMControlData.h"
#include "UI/SMUserWidget_HUD.h"
#include "UI/SMUserWidget_VictoryDefeat.h"
#include "Utilities/SMAssetPath.h"

ASMPlayerController::ASMPlayerController()
{
	static ConstructorHelpers::FObjectFinder<USMControlData> ControlDataRef(SMAssetPath::ControlData);
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

void ASMPlayerController::BeginPlay()
{
	Super::BeginPlay();

	InitControl();
}

void ASMPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	HUDWidget = CreateWidget<USMUserWidget_HUD>(this, Cast<UClass>(HUDWidgetClass));
	HUDWidget->AddToViewport(0);

	VictoryDefeatWidget = CreateWidget<USMUserWidget_VictoryDefeat>(this, Cast<UClass>(VictoryDefeatWidgetClass));
	VictoryDefeatWidget->AddToViewport(1);

	ASMPlayerState* SMPlayerState = GetPlayerState<ASMPlayerState>();
	if (ensure(SMPlayerState))
	{
		UAbilitySystemComponent* SourceASC = SMPlayerState->GetAbilitySystemComponent();
		HUDWidget->SetASC(SourceASC);
		VictoryDefeatWidget->SetASC(SourceASC);
	}
}

void ASMPlayerController::InitControl()
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
