// Copyright Surround, Inc. All Rights Reserved.


#include "SMPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "SMGamePlayerState.h"
#include "Blueprint/UserWidget.h"
#include "Characters/SMPlayerCharacter.h"
#include "Data/SMControlData.h"
#include "UI/SMUserWidget_HUD.h"
#include "UI/SMUserWidget_ScreenIndicator.h"
#include "UI/SMUserWidget_VictoryDefeat.h"
#include "Utilities/SMAssetPath.h"
#include "Utilities/SMLog.h"

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

	HUDWidget = CreateWidget<USMUserWidget_HUD>(this, HUDWidgetClass);
	HUDWidget->AddToViewport(0);

	VictoryDefeatWidget = CreateWidget<USMUserWidget_VictoryDefeat>(this, VictoryDefeatWidgetClass);
	VictoryDefeatWidget->AddToViewport(1);

	ASMGamePlayerState* SMPlayerState = GetPlayerState<ASMGamePlayerState>();
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
	SetInputMode(InputModeGameOnly);
}

void ASMPlayerController::AddScreendIndicator(AActor* TargetActor)
{
	if (HasAuthority())
	{
		return;
	}

	// 플레이어 캐릭터를 가진 경우에만 스크린 인디케이터를 추가합니다. 
	if (!Cast<ASMPlayerCharacter>(GetPawn()))
	{
		return;
	}

	// 인디케이터를 생성하고 타겟을 지정합니다.
	USMUserWidget_ScreenIndicator* OffScreendIndicator = CreateWidget<USMUserWidget_ScreenIndicator>(this, OffScreenIndicatorClass);
	OffScreendIndicator->AddToViewport(-1);
	OffScreendIndicator->SetTarget(TargetActor);
	OffScreenIndicators.Add(TargetActor, OffScreendIndicator);
}

void ASMPlayerController::RemoveScreenIndicator(AActor* TargetActor)
{
	if (HasAuthority())
	{
		return;
	}

	// 플레이어 캐릭터를 가진 경우에만 스크린 인디케이터를 제거합니다. 
	if (!Cast<ASMPlayerCharacter>(GetPawn()))
	{
		return;
	}

	// 인디케이터를 제거합니다. 중복 제거되도 문제는 없습니다.
	TObjectPtr<USMUserWidget_ScreenIndicator>* OffScreendIndicator = OffScreenIndicators.Find(TargetActor);
	if (!OffScreendIndicator)
	{
		return;
	}

	(*OffScreendIndicator)->RemoveFromParent();
	(*OffScreendIndicator)->ConditionalBeginDestroy();
	OffScreenIndicators.Remove(TargetActor);
}
