// Copyright Surround, Inc. All Rights Reserved.


#include "SMRoomPlayerController.h"

#include "StereoMixLog.h"

ASMRoomPlayerController::ASMRoomPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Type::Default;
}

void ASMRoomPlayerController::BeginPlay()
{
	Super::BeginPlay();

	RoomState = GetWorld()->GetGameStateChecked<ASMRoomState>();
	
	// SetInputMode(FInputModeUIOnly());
	UE_LOG(LogStereoMix, Log, TEXT("ASMRoomPlayerController::BeginPlay"));
}

void ASMRoomPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	RoomPlayerState = CastChecked<ASMRoomPlayerState>(PlayerState);
	RoomPlayerState->SetCurrentState(ERoomPlayerStateType::Unready);
	
	LoadingScreenWidget = CreateWidget<USMLoadingScreenWidget>(this, LoadingScreenWidgetClass);
	LoadingScreenWidget->SetLoadingText(FText::FromString(TEXT("로딩 중 입니다...")));
	LoadingScreenWidget->AddToViewport(10);
	
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, [this]
	{
		LoadingScreenWidget->HideLoadingScreen();
		RoomWidget = CreateWidget<USMRoomWidget>(this, RoomWidgetClass);
		RoomWidget->AddToViewport();
		RoomWidget->InitWidget(RoomState.Get(), RoomPlayerState.Get());
	}, 1.0f, false);

	UE_LOG(LogStereoMix, Log, TEXT("ASMRoomPlayerController::OnRep_PlayerState"));
}
