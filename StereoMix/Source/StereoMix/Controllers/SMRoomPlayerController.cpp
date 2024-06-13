// Copyright Surround, Inc. All Rights Reserved.


#include "SMRoomPlayerController.h"

#include "StereoMixLog.h"
#include "Games/Room/SMRoomMode.h"

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

void ASMRoomPlayerController::RequestImmediateStartGame_Implementation()
{
#if WITH_SERVER_CODE
	if (GetWorld() && GetWorld()->GetAuthGameMode())
	{
		ASMRoomMode* RoomMode = CastChecked<ASMRoomMode>(GetWorld()->GetAuthGameMode());
		if (RoomState.IsValid())
		{
			for (TObjectPtr<APlayerState> PS : RoomState->PlayerArray)
			{
				// 모든 플레이어가 팀에 소속되야 게임을 시작할 수 있습니다.
				const ASMRoomPlayerState* RoomPlayer = CastChecked<ASMRoomPlayerState>(PS);
				if (RoomPlayer->GetTeam() == ESMTeam::None)
				{
					return;
				}
			}

			// 팀 균형이 안맞아도 강제 시작
			RoomMode->StartGame();
		}
	}
#endif
}
