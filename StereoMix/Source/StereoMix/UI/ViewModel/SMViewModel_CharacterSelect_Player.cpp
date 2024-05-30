// Copyright Surround, Inc. All Rights Reserved.


#include "SMViewModel_CharacterSelect_Player.h"

#include "StereoMix.h"
#include "Player/SMPlayerState.h"

void USMViewModel_CharacterSelect_Player::InitializeViewModel(UWorld* InWorld)
{
	Super::InitializeViewModel(InWorld);
	UE_LOG(LogStereoMix, Verbose, TEXT("USMViewModel_CharacterSelect_Player::InitializeViewModel"));
	SetPlayerName(FText::FromName(TEXT("테스트 플레이어")));
}

void USMViewModel_CharacterSelect_Player::BindPlayerState(ASMPlayerState* PlayerState)
{
	OwningPlayerState = PlayerState;

	SetPlayerName(FText::FromName(TEXT("테스트 플레이어")));
	
	// SetBind(true);
	// SetPlayerName(FText::FromString(OwningPlayerState->GetPlayerName()));
	// OwningPlayerState->CharacterTypeChangedEvent.AddDynamic(this, &USMViewModel_CharacterSelect_Player::SetCharacterType);
}

void USMViewModel_CharacterSelect_Player::Reset()
{
	SetBind(false);
	SetPlayerName(FText::GetEmpty());
	SetCharacterType(ESMCharacterType::None);
}
