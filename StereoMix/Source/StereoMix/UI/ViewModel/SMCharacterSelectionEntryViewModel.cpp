// Copyright Surround, Inc. All Rights Reserved.


#include "SMCharacterSelectionEntryViewModel.h"

#include "StereoMix.h"
#include "Player/SMPlayerState.h"

void USMCharacterSelectionEntryViewModel::InitializeViewModel(UWorld* InWorld)
{
	Super::InitializeViewModel(InWorld);
	UE_LOG(LogStereoMix, Verbose, TEXT("USMViewModel_CharacterSelect_Player::InitializeViewModel"));
	SetPlayerName(FText::FromName(TEXT("테스트 플레이어")));
}

void USMCharacterSelectionEntryViewModel::BindPlayerState(ASMPlayerState* PlayerState)
{
	OwningPlayerState = PlayerState;

	SetPlayerName(FText::FromName(TEXT("테스트 플레이어")));
	
	// SetBind(true);
	// SetPlayerName(FText::FromString(OwningPlayerState->GetPlayerName()));
	// OwningPlayerState->CharacterTypeChangedEvent.AddDynamic(this, &USMViewModel_CharacterSelect_Player::SetCharacterType);
}

void USMCharacterSelectionEntryViewModel::Reset()
{
	SetBind(false);
	SetPlayerName(FText::GetEmpty());
	SetCharacterType(ESMCharacterType::None);
}
