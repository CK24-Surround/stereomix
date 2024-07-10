// Copyright Surround, Inc. All Rights Reserved.


#include "SMChatTypeButton.h"

#include "CommonTextBlock.h"

void USMChatTypeButton::SetChatTypeText(EChatType ChatType)
{
	switch (ChatType)
	{
		case EChatType::All:
			ChatTypeTextBlock->SetText(FText::FromString(TEXT("[All]")));
			break;
		case EChatType::Team:
			ChatTypeTextBlock->SetText(FText::FromString(TEXT("[Team]")));
			break;
	}
}
