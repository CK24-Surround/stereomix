// Copyright Surround, Inc. All Rights Reserved.


#include "SMViewModel_RoomPlayer.h"

#include "StereoMix.h"
#include "GameFramework/PlayerState.h"
#include "Utilities/SMLog.h"

USMViewModel_RoomPlayer::USMViewModel_RoomPlayer()
{
}

void USMViewModel_RoomPlayer::Init(ASMRoomPlayerState* TargetPlayer)
{
	if (OwningPlayerState.IsValid())
	{
		Remove();
	}
	OwningPlayerState = TargetPlayer;

	const FString& PlayerName = OwningPlayerState->GetPlayerName();
	
	bool bIsLocal = false;
	if (OwningPlayerState->GetOwningController())
	{
		bIsLocal = OwningPlayerState->GetOwningController()->IsLocalPlayerController();
	}
	NET_LOG(OwningPlayerState.Get(), Verbose, TEXT("USMViewModel_RoomPlayer::Init - %s, %d"), *PlayerName, bIsLocal)
	SetItem(FText::FromString(PlayerName), bIsLocal);
}

void USMViewModel_RoomPlayer::Remove()
{
	ResetItem();
	OwningPlayerState.Reset();
	NET_LOG(OwningPlayerState.Get(), Verbose, TEXT("USMViewModel_RoomPlayer::Remove"))
}

void USMViewModel_RoomPlayer::SetItem(const FText& PlayerName, const bool bIsLocalPlayer)
{
	NET_LOG(OwningPlayerState.Get(), Verbose, TEXT("USMViewModel_RoomPlayer::SetItem - false, %s, %d"), *PlayerName.ToString(), bIsLocalPlayer);
	// UE_MVVM_SET_PROPERTY_VALUE(bEmpty, false);
	SetIsEmpty(false);
	UE_MVVM_SET_PROPERTY_VALUE(PlayerNameText, PlayerName);
	UE_MVVM_SET_PROPERTY_VALUE(bLocalPlayer, bIsLocalPlayer);
}

void USMViewModel_RoomPlayer::ResetItem()
{
	NET_LOG(OwningPlayerState.Get(), Verbose, TEXT("USMViewModel_RoomPlayer::ResetItem - true, %s, false"), *PlayerNameText.ToString());
	// UE_MVVM_SET_PROPERTY_VALUE(bEmpty, true);
	SetIsEmpty(true);
	UE_MVVM_SET_PROPERTY_VALUE(PlayerNameText, FText());
	UE_MVVM_SET_PROPERTY_VALUE(bLocalPlayer, false);
}
