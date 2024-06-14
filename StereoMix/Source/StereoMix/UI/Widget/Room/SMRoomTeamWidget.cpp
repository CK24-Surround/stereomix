// Copyright Surround, Inc. All Rights Reserved.


#include "SMRoomTeamWidget.h"

#include "SMRoomPlayerEntry.h"
#include "SMRoomPlayerEntryItem.h"
#include "SMRoomWidget.h"
#include "StereoMixLog.h"

USMRoomTeamWidget::USMRoomTeamWidget()
{
	bApplyAlphaOnDisable = false;
}

void USMRoomTeamWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void USMRoomTeamWidget::InitWidget(USMRoomWidget* RoomWidget, const ESMTeam TargetTeam)
{
	if (!ensure(RoomWidget))
	{
		return;
	}
	if (!ensure(TargetTeam != ESMTeam::None))
	{
		return;
	}

	ParentRoomWidget = RoomWidget;
	Team = TargetTeam;

	const ASMRoomState* RoomState = RoomWidget->GetOwningRoomState();
	const TArray<ASMRoomPlayerState*>& TeamPlayers = RoomState->GetPlayersInTeam(TargetTeam);

	PlayerListView->ClearListItems();
	const int32 MaxPlayersInTeam = RoomWidget->GetOwningRoomState()->MaxPlayersInTeam;
	for (int i = 0; i < MaxPlayersInTeam; ++i)
	{
		USMRoomPlayerEntryItem* EntryItem = NewObject<USMRoomPlayerEntryItem>(this);
		EntryItem->Init(TeamPlayers.IsValidIndex(i) ? TeamPlayers[i] : nullptr);
		PlayerListView->AddItem(EntryItem);
	}

	// RoomWidget->GetOwningPlayerState()->TeamChangedEvent.AddDynamic(this, &USMRoomTeamWidget::OnLocalPlayerTeamChanged);
	RoomWidget->GetOwningPlayerState()->OnTeamChangeResponse.AddDynamic(this, &USMRoomTeamWidget::OnLocalPlayerTeamChangeResponse);

	RoomWidget->GetOwningRoomState()->OnTeamPlayersUpdated.AddDynamic(this, &USMRoomTeamWidget::OnTeamPlayersUpdated);

	PlayAnimationTimeRange(SelectAnim, 0.f, SelectAnimDuration.EndTime, 1, EUMGSequencePlayMode::Forward);
}

void USMRoomTeamWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (TeamCoverImage && TeamCoverTexture)
	{
		TeamCoverImage->SetBrushFromTexture(TeamCoverTexture);
	}

	if (TeamLogoImage && TeamLogoTexture)
	{
		TeamLogoImage->SetBrushFromTexture(TeamLogoTexture);
	}
}

void USMRoomTeamWidget::SetButtonState(const ERoomTeamSelectButtonState NewState)
{
	const ERoomTeamSelectButtonState PrevState = ButtonState;
	if (PrevState == NewState)
	{
		return;
	}

	ButtonState = NewState;
	// UE_LOG(LogStereoMixUI, Verbose, TEXT("[USMRoomTeamWidget] %s: SetButtonState: %s"), *UEnum::GetValueAsString(Team), *UEnum::GetValueAsString(NewState));
	switch (NewState)
	{
		case ERoomTeamSelectButtonState::Idle:
			if (PrevState == ERoomTeamSelectButtonState::Focused)
			{
				//QueuePlayAnimationTimeRange(SelectAnim, FocusAnimDuration.EndTime, 0.f, 1, EUMGSequencePlayMode::Reverse);
			}
			else if (PrevState == ERoomTeamSelectButtonState::Selected)
			{
				//QueuePlayAnimationTimeRange(SelectAnim, SelectAnimDuration.EndTime, 0.f, 1, EUMGSequencePlayMode::Reverse);
			}
			break;

		case ERoomTeamSelectButtonState::Focused:
			if (PrevState == ERoomTeamSelectButtonState::Idle)
			{
				//QueuePlayAnimationTimeRange(SelectAnim, 0.f, FocusAnimDuration.EndTime, 1, EUMGSequencePlayMode::Forward);
			}
			else if (PrevState == ERoomTeamSelectButtonState::Selected)
			{
				//QueuePlayAnimationTimeRange(SelectAnim, SelectAnimDuration.EndTime, FocusAnimDuration.EndTime, 1, EUMGSequencePlayMode::Reverse);
			}
			break;

		case ERoomTeamSelectButtonState::Selected:
			if (PrevState == ERoomTeamSelectButtonState::Idle)
			{
				//QueuePlayAnimationTimeRange(SelectAnim, 0.f, SelectAnimDuration.EndTime, 1, EUMGSequencePlayMode::Forward);
			}
			break;

		case ERoomTeamSelectButtonState::Disabled:
			if (PrevState == ERoomTeamSelectButtonState::Focused)
			{
				//QueuePlayAnimationTimeRange(SelectAnim, FocusAnimDuration.EndTime, 0.f, 1, EUMGSequencePlayMode::Reverse);
			}
			else if (PrevState == ERoomTeamSelectButtonState::Selected)
			{
				//QueuePlayAnimationTimeRange(SelectAnim, SelectAnimDuration.EndTime, 0.f, 1, EUMGSequencePlayMode::Reverse);
			}
			break;
		default:
			break;
	}
}

void USMRoomTeamWidget::HandleFocusReceived()
{
	Super::HandleFocusReceived();
	if (ButtonState != ERoomTeamSelectButtonState::Disabled && !ParentRoomWidget->bWaitForResponseTeamChange && ParentRoomWidget->GetOwningPlayerState()->GetTeam() != Team)
	{
		SetButtonState(ERoomTeamSelectButtonState::Focused);
	}
}

void USMRoomTeamWidget::HandleFocusLost()
{
	Super::HandleFocusLost();
	if (ButtonState != ERoomTeamSelectButtonState::Disabled && !ParentRoomWidget->bWaitForResponseTeamChange && ParentRoomWidget->GetOwningPlayerState()->GetTeam() != Team)
	{
		SetButtonState(ERoomTeamSelectButtonState::Idle);
	}
}

void USMRoomTeamWidget::NativeOnHovered()
{
	Super::NativeOnHovered();
	if (ButtonState != ERoomTeamSelectButtonState::Disabled && !ParentRoomWidget->bWaitForResponseTeamChange && ParentRoomWidget->GetOwningPlayerState()->GetTeam() != Team)
	{
		SetButtonState(ERoomTeamSelectButtonState::Focused);
	}
}

void USMRoomTeamWidget::NativeOnUnhovered()
{
	Super::NativeOnUnhovered();

	if (ButtonState != ERoomTeamSelectButtonState::Disabled && !ParentRoomWidget->bWaitForResponseTeamChange && ParentRoomWidget->GetOwningPlayerState()->GetTeam() != Team)
	{
		SetButtonState(ERoomTeamSelectButtonState::Idle);
	}
}

void USMRoomTeamWidget::NativeOnClicked()
{
	Super::NativeOnClicked();

	if (ButtonState != ERoomTeamSelectButtonState::Disabled && !ParentRoomWidget->bWaitForResponseTeamChange && ParentRoomWidget->GetOwningPlayerState()->GetTeam() != Team)
	{
		ParentRoomWidget->bWaitForResponseTeamChange = true;
		ParentRoomWidget->GetOwningPlayerState()->ChangeTeam(Team);
		OnSelected();
	}
}

void USMRoomTeamWidget::OnSelected()
{
	SetButtonState(ERoomTeamSelectButtonState::Selected);
}

void USMRoomTeamWidget::OnDeselected()
{
	SetButtonState(ERoomTeamSelectButtonState::Idle);
}

void USMRoomTeamWidget::OnLocalPlayerTeamChangeResponse(bool bSuccess, const ESMTeam NewTeam)
{
	if (!bSuccess)
	{
		return;
	}

	if (NewTeam == Team)
	{
		OnSelected();
	}
	else
	{
		OnDeselected();
	}
}

void USMRoomTeamWidget::OnTeamPlayersUpdated(const ESMTeam UpdatedTeam)
{
	if (UpdatedTeam != Team)
	{
		return;
	}

	UE_LOG(LogStereoMixUI, Verbose, TEXT("[USMRoomTeamWidget] %s: OnTeamPlayersUpdated"), *UEnum::GetValueAsString(Team));

	const TArray<ASMRoomPlayerState*>& TeamPlayers = ParentRoomWidget->GetOwningRoomState()->GetPlayersInTeam(Team);
	for (int i = 0; i < PlayerListView->GetNumItems(); ++i)
	{
		USMRoomPlayerEntryItem* EntryItem = CastChecked<USMRoomPlayerEntryItem>(PlayerListView->GetItemAt(i));
		EntryItem->Init(TeamPlayers.IsValidIndex(i) ? TeamPlayers[i] : nullptr);
		USMRoomPlayerEntry* EntryWidget = PlayerListView->GetEntryWidgetFromItem<USMRoomPlayerEntry>(EntryItem);
		if (EntryWidget)
		{
			EntryWidget->NativeOnListItemObjectSet(EntryItem);
		}
	}
	// PlayerListView->RequestRefresh();
}
