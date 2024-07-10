// Copyright Surround, Inc. All Rights Reserved.


#include "SMRoomPlayerController.h"

#include "Games/Room/SMRoomMode.h"
#include "Subsystem/SMBackgroundMusicSubsystem.h"
#include "Utilities/SMLog.h"

ASMRoomPlayerController::ASMRoomPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Type::Default;
}

void ASMRoomPlayerController::BeginPlay()
{
	Super::BeginPlay();
	RoomState = GetWorld()->GetGameStateChecked<ASMRoomState>();

	if (GetWorld()->GetGameViewport())
	{
		LoadingScreenWidget = CreateWidget<USMLoadingScreenWidget>(this, LoadingScreenWidgetClass);
		LoadingScreenWidget->SetLoadingText(FText::FromString(TEXT("로딩 중 입니다...")));
		LoadingScreenWidget->AddToViewport(10);
	}
}

void ASMRoomPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (GetWorld()->GetGameViewport())
	{
		if (!bTravelingToCharacterSelect)
		{
			GetGameInstance()->GetSubsystem<USMBackgroundMusicSubsystem>()->StopAndReleaseBackgroundMusic();
		}
	}
}

void ASMRoomPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	RoomPlayerState = CastChecked<ASMRoomPlayerState>(PlayerState);
	RoomPlayerState->SetCurrentState(ERoomPlayerStateType::Unready);
	RoomPlayerState->TeamChangedEvent.AddDynamic(this, &ASMRoomPlayerController::OnTeamChanged);

	OnCompleteLoading();
}

void ASMRoomPlayerController::PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel)
{
	Super::PreClientTravel(PendingURL, TravelType, bIsSeamlessTravel);

	const FURL NextURL = FURL(&GetWorld()->URL, *PendingURL, TravelType);
	bTravelingToCharacterSelect = NextURL.Map == TEXT("/Game/StereoMix/Levels/CharacterSelect/L_CharacterSelect");
	NET_LOG(this, Verbose, TEXT("bTravelingToCharacterSelect: %d"), bTravelingToCharacterSelect)
}

void ASMRoomPlayerController::OnCompleteLoading()
{
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, [this] {
		RoomPlayerState->SetCurrentState(ERoomPlayerStateType::Unready);
		if (GetWorld()->GetGameViewport())
		{
			LoadingScreenWidget->HideLoadingScreen();
			RoomWidget = CreateWidget<USMRoomWidget>(this, RoomWidgetClass);
			RoomWidget->AddToViewport();
			RoomWidget->InitWidget(RoomState.Get(), RoomPlayerState.Get());
			GetGameInstance()->GetSubsystem<USMBackgroundMusicSubsystem>()->PlayTeamBackgroundMusic(ESMTeam::None);
		}
	}, 1.0f, false);
}

void ASMRoomPlayerController::OnTeamChanged(ESMTeam NewTeam)
{
	if (GetWorld()->GetGameViewport())
	{
		GetGameInstance()->GetSubsystem<USMBackgroundMusicSubsystem>()->PlayTeamBackgroundMusic(NewTeam);
	}
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

			if (RoomState->GetTeamEdmPlayers().Num() == RoomState->GetTeamFutureBassPlayers().Num())
			{
				RoomMode->StartGame();
			}
		}
	}
#endif
}
