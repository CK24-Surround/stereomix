// Copyright Surround, Inc. All Rights Reserved.

#include "SMChatWidget.h"

#include "CommonListView.h"
#include "SMChatEntryItem.h"
#include "SMChatTypeButton.h"
#include "Components/EditableTextBox.h"
#include "Controllers/SMPlayerController.h"
#include "Games/SMPlayerState.h"
#include "Input/CommonUIInputTypes.h"

USMChatWidget::USMChatWidget()
{
}

void USMChatWidget::AddMessage(const bool bTeamChat, const FString& Name, const FString& Message)
{
	USMChatEntryItem* NewChat = NewObject<USMChatEntryItem>();
	NewChat->bTeamChat = bTeamChat;
	NewChat->Name = Name;
	NewChat->Message = Message;

	MessageListView->AddItem(NewChat);
	MessageListView->ScrollToBottom();
}

void USMChatWidget::SetChatType(EChatType NewChatType)
{
	ChatType = NewChatType;
	ChatTypeButton->SetChatTypeText(ChatType);
}

void USMChatWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (bAlwaysActivateWidget)
	{
		bAutoActivate = true;
	}
}

void USMChatWidget::NativeConstruct()
{
	Super::NativeConstruct();

	OwningPlayerController = CastChecked<ASMPlayerController>(GetOwningPlayer());
	OwningPlayerController->OnChatReceived.AddDynamic(this, &USMChatWidget::OnChatReceivedFromServer);
	OwningPlayerController->OnTeamChatReceived.AddDynamic(this, &USMChatWidget::OnTeamChatReceivedFromServer);

	OwningPlayerState = CastChecked<ASMPlayerState>(GetOwningPlayerState(true));
	OwningPlayerState->TeamChangedEvent.AddDynamic(this, &USMChatWidget::OnOwningPlayerTeamChanged);
	bCanChangeChatType = OwningPlayerState->GetTeam() != ESMTeam::None;

	InitActivatableWidgets();

	SetChatType(EChatType::All);
	MessageInputBox->SetText(FText::GetEmpty());

	if (bAlwaysActivateWidget)
	{
		SetVisibilityForActivatableWidgets(ESlateVisibility::Visible);
	}
	else
	{
		SetVisibilityForActivatableWidgets(bHideWidgetsOnDeactivate ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
	}

	ChatTypeButtonClickDelegateHandle = ChatTypeButton->OnClicked().AddUObject(this, &USMChatWidget::ChangeChatType);
}

void USMChatWidget::NativeDestruct()
{
	Super::NativeDestruct();

	UnregisterInputComponent();
	OwningPlayerController->OnChatReceived.RemoveDynamic(this, &USMChatWidget::OnChatReceivedFromServer);
	OwningPlayerController->OnTeamChatReceived.RemoveDynamic(this, &USMChatWidget::OnTeamChatReceivedFromServer);
	OwningPlayerState->TeamChangedEvent.RemoveDynamic(this, &USMChatWidget::OnOwningPlayerTeamChanged);
	ChatTypeButton->OnClicked().Remove(ChatTypeButtonClickDelegateHandle);
}

void USMChatWidget::NativeOnActivated()
{
	Super::NativeOnActivated();
	SetVisibilityForActivatableWidgets(ESlateVisibility::Visible);
	MessageInputBox->SetIsEnabled(true);
	MessageInputBox->OnTextCommitted.AddUniqueDynamic(this, &USMChatWidget::OnMessageInputBoxCommitted);
}

void USMChatWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
	MessageListView->ScrollToBottom();

	MessageInputBox->OnTextCommitted.RemoveDynamic(this, &USMChatWidget::OnMessageInputBoxCommitted);
	MessageInputBox->SetIsEnabled(false);

	if (bHideWidgetsOnDeactivate)
	{
		SetVisibilityForActivatableWidgets(ESlateVisibility::Hidden);
	}
}

UWidget* USMChatWidget::NativeGetDesiredFocusTarget() const
{
	return MessageInputBox;
}

TOptional<FUIInputConfig> USMChatWidget::GetDesiredInputConfig() const
{
	FUIInputConfig InputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture, EMouseLockMode::DoNotLock);
	InputConfig.bIgnoreLookInput = true;
	return InputConfig;
}

FReply USMChatWidget::NativeOnKeyChar(const FGeometry& InGeometry, const FCharacterEvent& InCharEvent)
{
	if (InCharEvent.GetCharacter() == '\t' && !InCharEvent.IsRepeat())
	{
		ChangeChatType();
		return FReply::Handled();
	}

	return Super::NativeOnKeyChar(InGeometry, InCharEvent);
}

void USMChatWidget::OpenChat()
{
	if (!IsActivated())
	{
		ActivateWidget();
	}
	else
	{
		MessageInputBox->SetKeyboardFocus();
	}
}

void USMChatWidget::ChangeChatType()
{
	if (bCanChangeChatType)
	{
		SetChatType(GetChatType() == EChatType::All ? EChatType::Team : EChatType::All);
	}
}

void USMChatWidget::OnOwningPlayerTeamChanged(ESMTeam NewTeam)
{
	if (NewTeam != ESMTeam::None)
	{
		bCanChangeChatType = true;
	}
	else
	{
		bCanChangeChatType = false;
		SetChatType(EChatType::All);
	}
}

void USMChatWidget::OnChatReceivedFromServer(const FString& Name, const FString& ChatMessage)
{
	AddMessage(false, Name, ChatMessage);
}

void USMChatWidget::OnTeamChatReceivedFromServer(const FString& Name, const FString& ChatMessage)
{
	AddMessage(true, Name, ChatMessage);
}

void USMChatWidget::OnMessageInputBoxCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (!IsActivated())
	{
		return;
	}

	if (CommitMethod == ETextCommit::OnEnter)
	{
		if (!Text.IsEmpty())
		{
			switch (ChatType)
			{
				case EChatType::All:
					OwningPlayerController->ServerSendChat(Text.ToString());
					break;

				case EChatType::Team:
					OwningPlayerController->ServerSendTeamChat(Text.ToString());
					break;
			}
		}
	}

	MessageInputBox->SetText(FText::GetEmpty());

	if (!bAlwaysActivateWidget)
	{
		DeactivateWidget();
	}
}

void USMChatWidget::InitActivatableWidgets()
{
	for (FWidgetChild& TargetWidget : ActivatableWidgets)
	{
		TargetWidget.Resolve(WidgetTree);
	}
}

void USMChatWidget::SetVisibilityForActivatableWidgets(ESlateVisibility InVisibility)
{
	MessageListView->SetScrollbarVisibility(InVisibility);

	if (InVisibility == ESlateVisibility::Visible)
	{
		MessageInputBox->SetIsEnabled(true);
	}
	else
	{
		MessageInputBox->SetIsEnabled(false);
	}

	for (FWidgetChild& TargetWidget : ActivatableWidgets)
	{
		if (TargetWidget.GetWidget())
		{
			TargetWidget.GetWidget()->SetVisibility(InVisibility);
		}
	}
}
