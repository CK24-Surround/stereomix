// Copyright Surround, Inc. All Rights Reserved.


#include "SMRoomPlayerViewModel.h"

#include "StereoMix.h"
#include "GameFramework/PlayerState.h"
#include "Utilities/SMLog.h"

USMRoomPlayerViewModel::USMRoomPlayerViewModel()
{
}

void USMRoomPlayerViewModel::Init(ASMRoomPlayerState* TargetPlayer)
{
	if (TargetPlayer == nullptr)
	{
		Remove();
		return;
	}
	
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
	SetItem(FText::FromString(PlayerName), bIsLocal);
}

void USMRoomPlayerViewModel::Remove()
{
	ResetItem();
	OwningPlayerState.Reset();
}

void USMRoomPlayerViewModel::SetItem(const FText& PlayerName, const bool bIsLocalPlayer)
{
	NET_LOG(OwningPlayerState.Get(), Verbose, TEXT("[Room_Player_VM] Player %s set item. localplayer: %s"), *PlayerName.ToString(), bIsLocalPlayer ? TEXT("true") : TEXT("false"))
	SetIsEmpty(false);
	UE_MVVM_SET_PROPERTY_VALUE(PlayerNameText, PlayerName);
	UE_MVVM_SET_PROPERTY_VALUE(bLocalPlayer, bIsLocalPlayer);
}

void USMRoomPlayerViewModel::ResetItem()
{
	SetIsEmpty(true);
	UE_MVVM_SET_PROPERTY_VALUE(PlayerNameText, FText());
	UE_MVVM_SET_PROPERTY_VALUE(bLocalPlayer, false);
}
