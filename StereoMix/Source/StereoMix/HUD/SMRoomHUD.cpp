// Copyright Surround, Inc. All Rights Reserved.


#include "SMRoomHUD.h"

#include "StereoMix.h"

ASMRoomHUD::ASMRoomHUD()
{
}

void ASMRoomHUD::BeginPlay()
{
	Super::BeginPlay();

	if (MainWidget)
	{
		if (USMViewModel_Room* ViewModel = GetViewModelFromWidget<USMViewModel_Room>(MainWidget, TEXT("SMViewModel_Room")))
		{
			RoomViewModel = ViewModel;
		}
	}

	if (!RoomViewModel)
	{
		UE_LOG(LogStereoMix, Error, TEXT("RoomViewModel is not set."))
		return;
	}
	
	RoomState = GetWorld()->GetGameState<ASMRoomState>();
	if (RoomState)
	{
		RoomViewModel->BindGameState(RoomState);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Current GameMode does not have a RoomState."))
	}
}
