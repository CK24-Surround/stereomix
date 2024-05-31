// Copyright Surround, Inc. All Rights Reserved.


#include "SMRoomViewModel.h"

// 클립보드
#if PLATFORM_WINDOWS
#include "Windows/WindowsPlatformApplicationMisc.h"
#elif PLATFORM_LINUX
#include "Linux//LinuxPlatformApplicationMisc.h"
#endif

USMRoomViewModel::USMRoomViewModel()
{
}

void USMRoomViewModel::BindGameState(ASMRoomState* InRoomState)
{
	ensure(InRoomState);

	OwningRoomState = InRoomState;
	ConnectionSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USMClientConnectionSubsystem>();

	SetTeamA(NewObject<USMTeamViewModel>(GetWorld()));
	TeamA->SetTeam(ESMTeam::FutureBass);
	TeamA->BindGameState(InRoomState);

	SetTeamB(NewObject<USMTeamViewModel>(GetWorld()));
	TeamB->SetTeam(ESMTeam::EDM);
	TeamB->BindGameState(InRoomState);

	SetRoomCodeText(FText::FromString(OwningRoomState->ShortRoomCode));
	SetPlayerCount(OwningRoomState->PlayerArray.Num());

	OwningRoomState->OnPlayerJoined.AddDynamic(this, &USMRoomViewModel::OnPlayerJoined);
	OwningRoomState->OnPlayerLeft.AddDynamic(this, &USMRoomViewModel::OnPlayerLeft);
}

void USMRoomViewModel::CopyCode()
{
	if (OwningRoomState.IsValid())
	{
		FPlatformApplicationMisc::ClipboardCopy(*OwningRoomState->ShortRoomCode);
	}
}

void USMRoomViewModel::QuitRoom()
{
	if (const UWorld* World = GetWorld())
	{
		World->GetGameInstance()->ReturnToMainMenu();
	}
}

void USMRoomViewModel::OnPlayerJoined(ASMPlayerState* JoinedPlayer)
{
	if (OwningRoomState.IsValid())
	{
		SetPlayerCount(OwningRoomState->PlayerArray.Num());
	}
}

void USMRoomViewModel::OnPlayerLeft(ASMPlayerState* LeftPlayer)
{
	if (OwningRoomState.IsValid())
	{
		SetPlayerCount(OwningRoomState->PlayerArray.Num());
	}
}
