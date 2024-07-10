// Copyright Surround, Inc. All Rights Reserved.


#include "SMChatEntryWidget.h"

#include "CommonTextBlock.h"
#include "SMChatEntryItem.h"

void USMChatEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	const USMChatEntryItem* EntryItem = CastChecked<USMChatEntryItem>(ListItemObject);

	NameTextBlock->SetStyle(EntryItem->bTeamChat ? TeamChatTextStyle : AllChatTextStyle);
	NameTextBlock->SetText(FText::FromString(FString::Printf(TEXT("[%s]"), *EntryItem->Name)));
	MessageTextBlock->SetStyle(EntryItem->bTeamChat ? TeamChatTextStyle : AllChatTextStyle);
	MessageTextBlock->SetText(FText::FromString(EntryItem->Message));
}
