// Copyright Surround, Inc. All Rights Reserved.


#include "SMRoomWidget.h"

#include "StereoMixLog.h"
#include "Controllers/SMRoomPlayerController.h"
#include "UI/Widget/Chat/SMChatWidget.h"

USMRoomWidget::USMRoomWidget()
{
	bAutoActivate = true;
}

void USMRoomWidget::InitWidget(ASMRoomState* RoomState, ASMRoomPlayerState* PlayerState)
{
	if (ensure(RoomState))
	{
		OwningRoomState = RoomState;
	}
	if (ensure(PlayerState))
	{
		OwningPlayerState = PlayerState;
	}

	// OwningPlayerState->TeamChangedEvent.AddDynamic(this, &USMRoomWidget::OnLocalPlayerTeamChanged);
	OwningPlayerState->OnTeamChangeResponse.AddDynamic(this, &USMRoomWidget::OnTeamChangeResponse);

	OwningRoomState->OnPlayerJoined.AddDynamic(this, &USMRoomWidget::OnPlayerJoin);
	OwningRoomState->OnPlayerLeft.AddDynamic(this, &USMRoomWidget::OnPlayerLeft);
	OwningRoomState->OnTeamPlayersUpdated.AddDynamic(this, &USMRoomWidget::OnTeamPlayersUpdated);

	EdmTeamWidget->InitWidget(this, ESMTeam::EDM);
	FutureBassTeamWidget->InitWidget(this, ESMTeam::FutureBass);
	CodeCopyButton->SetRoomCode(RoomState->GetRoomCode());

	UpdatePlayerCount();

	PlayAnimationForward(TransitionAnim);
}

void USMRoomWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GameStartButton->OnClicked().AddUObject(this, &USMRoomWidget::OnGameStartButtonClicked);
	QuitButton->OnClicked().AddUObject(this, &USMRoomWidget::OnQuitButtonClicked);
}

void USMRoomWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

bool USMRoomWidget::NativeOnHandleBackAction()
{
	if (bIsBackHandler)
	{
		UE_LOG(LogStereoMixUI, Warning, TEXT("[USMRoomWidget] NativeOnHandleBackAction: Back action handled"))
	}
	return false;
}

TOptional<FUIInputConfig> USMRoomWidget::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture, EMouseLockMode::DoNotLock);
}

void USMRoomWidget::UpdatePlayerCount() const
{
	if (GetOwningRoomState())
	{
		const int32 NumPlayers = GetOwningRoomState()->PlayerArray.Num();
		const int32 MaxPlayers = GetOwningRoomState()->MaxPlayersInGame;

		PlayerCountTextBlock->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), NumPlayers, MaxPlayers)));
	}
}

void USMRoomWidget::OnGameStartButtonClicked()
{
	// TODO: 인원 수가 맞는 지 검증
	ASMRoomPlayerController* RoomPlayerController = CastChecked<ASMRoomPlayerController>(GetOwningPlayer());
	RoomPlayerController->RequestImmediateStartGame();
}

void USMRoomWidget::OnQuitButtonClicked()
{
	if (!GetOwningPlayer())
	{
		UE_LOG(LogStereoMixUI, Error, TEXT("[USMRoomWidget] OnQuitButtonClicked: GetOwningPlayer() is nullptr"))
		return;
	}
	GetGameInstance()->ReturnToMainMenu();
}

void USMRoomWidget::OnPlayerJoin(ASMPlayerState* JoinedPlayer)
{
	UpdatePlayerCount();
}

void USMRoomWidget::OnPlayerLeft(ASMPlayerState* LeftPlayer)
{
	UpdatePlayerCount();
}

void USMRoomWidget::OnTeamChangeResponse(const bool bSuccess, const ESMTeam NewTeam)
{
	bWaitForResponseTeamChange = false;
	if (!bSuccess)
	{
		UE_LOG(LogStereoMixUI, Error, TEXT("[USMRoomWidget] OnTeamChangeResponse: Failed to change team to %s"), *UEnum::GetValueAsString(NewTeam))
		return;
	}
	UE_LOG(LogStereoMixUI, Verbose, TEXT("[USMRoomWidget] OnLocalPlayerTeamChanged: %s"), *UEnum::GetValueAsString(NewTeam))

	const ESMTeam PrevTeam = CurrentTeam;
	CurrentTeam = NewTeam;

	if (PrevTeam == ESMTeam::EDM)
	{
		PlayAnimationReverse(EdmSelectAnim);
	}
	else if (PrevTeam == ESMTeam::FutureBass)
	{
		PlayAnimationReverse(FutureBassSelectAnim);
	}

	if (NewTeam == ESMTeam::EDM)
	{
		// 임시로 이미지 고정
		// UTexture2D* NewCharacterTexture = EdmCharacterTextures[FMath::RandRange(0, EdmCharacterTextures.Num() - 1)];
		// EdmCharacterImage->SetBrushFromTexture(NewCharacterTexture);
		PlayAnimationForward(EdmSelectAnim);
	}
	else if (NewTeam == ESMTeam::FutureBass)
	{
		// 임시로 이미지 고정
		// UTexture2D* NewCharacterTexture = FutureBassCharacterTextures[FMath::RandRange(0, FutureBassCharacterTextures.Num() - 1)];
		// FutureBassCharacterImage->SetBrushFromTexture(NewCharacterTexture);
		PlayAnimationForward(FutureBassSelectAnim);
	}
}

void USMRoomWidget::OnTeamPlayersUpdated(ESMTeam UpdatedTeam)
{
}

void USMRoomWidget::OpenChatInput()
{
	if (!ChatWidget->IsActivated())
	{
		ChatWidget->ActivateWidget();
	}
}
