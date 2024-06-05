// Copyright Surround, Inc. All Rights Reserved.


#include "SMRoomPlayerEntry.h"

#include "SMRoomPlayerEntryItem.h"

const FString& USMRoomPlayerEntry::GetPlayerName() const
{
	return PlayerNameTextBlock->GetText().ToString();
}

bool USMRoomPlayerEntry::IsLocalPlayer() const
{
	return OwningPlayerState.IsValid() && GetOwningPlayer()->PlayerState == OwningPlayerState;
}

void USMRoomPlayerEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	const USMRoomPlayerEntryItem* EntryItem = CastChecked<USMRoomPlayerEntryItem>(ListItemObject);
	ASMRoomPlayerState* PlayerState = EntryItem->GetOwningPlayerState();
	if (!ensure(GetOwningPlayer()))
	{
		return;
	}

	if (PlayerState)
	{
		const FString& PlayerName = PlayerState->GetPlayerName();
		const bool bLocalPlayer = GetOwningPlayer()->PlayerState == PlayerState;

		OwningPlayerState = PlayerState;
		BackgroundImage->SetBrushFromTexture(BackgroundOnFilled);
		PlayerNameTextBlock->SetText(FText::FromString(PlayerName));
		PlayerNameTextBlock->SetStyle(bLocalPlayer ? LocalPlayerNameTextStyle : RemotePlayerNameTextStyle);
	}
	else
	{
		OwningPlayerState.Reset();
		BackgroundImage->SetBrushFromTexture(BackgroundOnEmpty);
		PlayerNameTextBlock->SetText(FText::GetEmpty());
	}
}

void USMRoomPlayerEntry::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (BackgroundImage && BackgroundOnFilled)
	{
		BackgroundImage->SetBrushFromTexture(BackgroundOnFilled);
	}

	if (PlayerNameTextBlock && LocalPlayerNameTextStyle)
	{
		PlayerNameTextBlock->SetStyle(LocalPlayerNameTextStyle);
	}
}
