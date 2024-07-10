// Copyright Surround, Inc. All Rights Reserved.


#include "SMCharacterSelectWidget.h"

#include "FMODBlueprintStatics.h"
#include "Games/SMCountdownTimerComponent.h"
#include "SMCharacterSelectPlayerEntryItem.h"

void USMCharacterSelectWidget::InitWidget(ASMCharacterSelectState* CharacterSelectState, ASMCharacterSelectPlayerState* PlayerState)
{
	if (ensure(CharacterSelectState))
	{
		OwningCharacterSelectState = CharacterSelectState;
	}
	if (ensure(PlayerState))
	{
		OwningPlayerState = PlayerState;
	}

	const ESMTeam Team = OwningPlayerState->GetTeam();
	if (Team == ESMTeam::EDM)
	{
		Background1Image->SetBrushFromTexture(EdmBackgroundData.Background1);
		Background2Image->SetBrushFromTexture(EdmBackgroundData.Background2);
		Background3Image->SetBrushFromTexture(EdmBackgroundData.Background3);
	}
	else if (Team == ESMTeam::FutureBass)
	{
		Background1Image->SetBrushFromTexture(FutureBassBackgroundData.Background1);
		Background2Image->SetBrushFromTexture(FutureBassBackgroundData.Background2);
		Background3Image->SetBrushFromTexture(FutureBassBackgroundData.Background3);
	}

	ElectricGuitarSelectButton->InitWidget(this);
	PianoSelectButton->InitWidget(this);
	BassSelectButton->InitWidget(this);

	UpdatePlayerList();
	OnCountdownTick();
	CharacterPreview->SetPreviewImage(ESMTeam::None, ESMCharacterType::None);

	OwningPlayerState->OnCharacterChangeResponse.AddDynamic(this, &USMCharacterSelectWidget::OnLocalPlayerCharacterChangeResponse);

	OwningCharacterSelectState->OnCurrentStateChanged.AddDynamic(this, &USMCharacterSelectWidget::OnCharacterSelectStateChanged);
	OwningCharacterSelectState->OnPlayerJoined.AddDynamic(this, &USMCharacterSelectWidget::OnPlayerJoin);
	OwningCharacterSelectState->OnPlayerLeft.AddDynamic(this, &USMCharacterSelectWidget::OnPlayerLeft);
	OwningCharacterSelectState->OnPlayerCharacterChanged.AddDynamic(this, &USMCharacterSelectWidget::OnPlayerCharacterChanged);

	OwningCharacterSelectState->GetCountdownTimer()->OnCountdownTick.AddDynamic(this, &USMCharacterSelectWidget::OnCountdownTick);

	PlayAnimationForward(TransitionAnim);
}

void USMCharacterSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void USMCharacterSelectWidget::UpdatePlayerList() const
{
	const ASMCharacterSelectState* CharacterSelectState = GetOwningCharacterSelectState();
	if (!CharacterSelectState)
	{
		return;
	}

	TArray<TObjectPtr<APlayerState>> Players = CharacterSelectState->PlayerArray;
	PlayerListView->ClearListItems();

	const ESMTeam LocalPlayerTeam = GetOwningPlayerState()->GetTeam();
	for (TObjectPtr<APlayerState> PlayerState : Players)
	{
		ASMCharacterSelectPlayerState* TargetPlayerState = Cast<ASMCharacterSelectPlayerState>(PlayerState);
		if (TargetPlayerState && TargetPlayerState->GetTeam() == LocalPlayerTeam)
		{
			USMCharacterSelectPlayerEntryItem* EntryItem = NewObject<USMCharacterSelectPlayerEntryItem>();
			EntryItem->Init(TargetPlayerState);
			PlayerListView->AddItem(EntryItem);
		}
	}
}

void USMCharacterSelectWidget::OnCharacterSelectStateChanged(const ECharacterSelectionStateType NewCharacterSelectionState)
{
	if (CountdownSound && NewCharacterSelectionState == ECharacterSelectionStateType::End)
	{
		UFMODBlueprintStatics::PlayEvent2D(GetWorld(), CountdownSound, true);
	}
}

void USMCharacterSelectWidget::OnLocalPlayerCharacterChangeResponse(bool bSuccess, ESMCharacterType NewCharacterType)
{
	if (!bSuccess)
	{
		return;
	}

	CharacterPreview->SetPreviewImage(GetOwningPlayerState()->GetTeam(), NewCharacterType);
}

void USMCharacterSelectWidget::OnPlayerCharacterChanged(ASMPlayerState* Player, ESMCharacterType NewCharacter)
{
	if (Player->GetTeam() != OwningPlayerState->GetTeam())
	{
		return;
	}

	UpdatePlayerList();
}

void USMCharacterSelectWidget::OnPlayerJoin(ASMPlayerState* JoinedPlayer)
{
	UpdatePlayerList();
}

void USMCharacterSelectWidget::OnPlayerLeft(ASMPlayerState* LeftPlayer)
{
	UpdatePlayerList();
}

void USMCharacterSelectWidget::OnCountdownTick()
{
	const int32 TotalTime = OwningCharacterSelectState->GetCountdownTimer()->GetInitTime();
	const int32 RemainingTime = OwningCharacterSelectState->GetCountdownTimer()->GetRemainingTime();
	CountdownTextBlock->SetText(FText::FromString(FString::Printf(TEXT("%d"), RemainingTime)));
	CountdownProgressBar->SetPercent(RemainingTime / static_cast<float>(TotalTime));
}
