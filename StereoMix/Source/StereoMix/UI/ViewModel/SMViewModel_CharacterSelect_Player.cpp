// Copyright Surround, Inc. All Rights Reserved.


#include "SMViewModel_CharacterSelect_Player.h"

#include "Player/SMPlayerState.h"

void USMViewModel_CharacterSelect_Player::BindPlayerState(ASMPlayerState* PlayerState)
{
	OwningPlayerState = PlayerState;

	SetBind(true);
	SetPlayerName(FText::FromString(OwningPlayerState->GetPlayerName()));
	OwningPlayerState->CharacterTypeChangedEvent.AddDynamic(this, &USMViewModel_CharacterSelect_Player::SetCharacterType);
}

void USMViewModel_CharacterSelect_Player::Reset()
{
	SetBind(false);
	SetPlayerName(FText::GetEmpty());
	SetCharacterType(ESMCharacterType::None);
}
