// Copyright Surround, Inc. All Rights Reserved.


#include "SMViewModel_RoomShortCodePopup.h"

#include "MVVMGameSubsystem.h"
#include "MVVMSubsystem.h"
#include "Connection/SMClientConnectionSubsystem.h"

void USMViewModel_RoomShortCodePopup::EnterRoom()
{
	if (USMClientConnectionSubsystem* ClientConnection = GetWorld()->GetGameInstance()->GetSubsystem<USMClientConnectionSubsystem>())
	{
		ClientConnection->JoinRoomUsingShortCode(GetRoomShortCode().ToString());
	}
}

void USMViewModel_RoomShortCodePopup::Cancel()
{
}
