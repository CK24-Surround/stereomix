// Copyright Surround, Inc. All Rights Reserved.


#include "SMViewModel_Room.h"

// 클립보드
#if PLATFORM_WINDOWS
#include "Windows/WindowsPlatformApplicationMisc.h"
#elif PLATFORM_LINUX
#include "Linux//LinuxPlatformApplicationMisc.h"
#endif

USMViewModel_Room::USMViewModel_Room()
{
}

void USMViewModel_Room::BindGameState(ASMRoomState* InRoomState)
{
	ensure(InRoomState);

	OwningRoomState = InRoomState;
	ConnectionSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USMClientConnectionSubsystem>();

	SetTeamA(NewObject<USMViewModel_RoomTeam>(GetWorld()));
	TeamA->SetTeam(ESMTeam::FutureBass);
	TeamA->BindGameState(InRoomState);

	SetTeamB(NewObject<USMViewModel_RoomTeam>(GetWorld()));
	TeamB->SetTeam(ESMTeam::EDM);
	TeamB->BindGameState(InRoomState);

	SetRoomCodeText(FText::FromString(OwningRoomState->ShortRoomCode));
	SetPlayerCount(OwningRoomState->PlayerArray.Num());

	OwningRoomState->OnPlayerJoined.AddDynamic(this, &USMViewModel_Room::OnPlayerJoined);
	OwningRoomState->OnPlayerLeft.AddDynamic(this, &USMViewModel_Room::OnPlayerLeft);
}

void USMViewModel_Room::CopyCode()
{
	if (OwningRoomState.IsValid())
	{
		FPlatformApplicationMisc::ClipboardCopy(*OwningRoomState->ShortRoomCode);
	}
}

void USMViewModel_Room::QuitRoom()
{
	if (const UWorld* World = GetWorld())
	{
		World->GetGameInstance()->ReturnToMainMenu();
	}
}

void USMViewModel_Room::OnPlayerJoined(ASMPlayerState* JoinedPlayer)
{
	if (OwningRoomState.IsValid())
	{
		SetPlayerCount(OwningRoomState->PlayerArray.Num());
	}
}

void USMViewModel_Room::OnPlayerLeft(ASMPlayerState* LeftPlayer)
{
	if (OwningRoomState.IsValid())
	{
		SetPlayerCount(OwningRoomState->PlayerArray.Num());
	}
}
