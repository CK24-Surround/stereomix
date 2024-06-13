// Copyright Surround, Inc. All Rights Reserved.


#include "SMCharacterSelectPlayerEntry.h"

#include "SMCharacterSelectPlayerEntryItem.h"

void USMCharacterSelectPlayerEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	const USMCharacterSelectPlayerEntryItem* EntryItem = CastChecked<USMCharacterSelectPlayerEntryItem>(ListItemObject);
	ASMCharacterSelectPlayerState* PlayerState = EntryItem->GetOwningPlayerState();
	if (!ensure(GetOwningPlayer()))
	{
		return;
	}

	if (PlayerState)
	{
		const FString& PlayerName = PlayerState->GetPlayerName();
		const ESMCharacterType CharacterType = PlayerState->GetCharacterType();
		const bool bLocalPlayer = GetOwningPlayer()->PlayerState == PlayerState;

		PlayerNameTextBlock->SetStyle(bLocalPlayer ? LocalPlayerNameTextStyle : RemotePlayerNameTextStyle);
		PlayerNameTextBlock->SetText(FText::FromString(PlayerName));

		switch (CharacterType)
		{
			case ESMCharacterType::None:
				SetReady(false);
				CharacterNameTextBlock->SetText(FText::FromString(TEXT("선택 중")));
				break;

			case ESMCharacterType::ElectricGuitar:
				SetReady(true);
				CharacterNameTextBlock->SetText(FText::FromString(TEXT("일렉기타")));
				break;

			case ESMCharacterType::Piano:
				SetReady(true);
				CharacterNameTextBlock->SetText(FText::FromString(TEXT("피아노")));
				break;

			case ESMCharacterType::Bass:
				SetReady(true);
				CharacterNameTextBlock->SetText(FText::FromString(TEXT("베이스")));
				break;
		}
	}
}

void USMCharacterSelectPlayerEntry::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void USMCharacterSelectPlayerEntry::SetReady(const bool bReady)
{
	ReadyIconSection->SetVisibility(bReady ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	NotReadyIconSection->SetVisibility(bReady ? ESlateVisibility::Hidden : ESlateVisibility::HitTestInvisible);
}
