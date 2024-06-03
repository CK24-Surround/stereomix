// Copyright Surround, Inc. All Rights Reserved.


#include "SMRoomPlayerState.h"

#include "SMRoomPlayerController.h"
#include "Games/SMRoomState.h"

ASMRoomPlayerState::ASMRoomPlayerState()
{
}

void ASMRoomPlayerState::BeginPlay()
{
	Super::BeginPlay();
	RoomState = GetWorld()->GetGameStateChecked<ASMRoomState>();
}

void ASMRoomPlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ASMRoomPlayerState::ChangeTeam_Implementation(ESMTeam NewTeam)
{
	SetTeam(NewTeam);
}

bool ASMRoomPlayerState::CanChangeTeam(const ESMTeam NewTeam) const
{
	if (GetTeam() == NewTeam)
	{
		return false;
	}

	if (RoomState.IsValid())
	{
		switch (NewTeam)
		{
		case ESMTeam::None:
			return true;
		case ESMTeam::EDM:
			return RoomState->GetTeamEdmPlayers().Num() < RoomState->ReplicatedMaxPlayersInTeam;
		case ESMTeam::FutureBass:
			return RoomState->GetTeamFutureBassPlayers().Num() < RoomState->ReplicatedMaxPlayersInTeam;
		default:
			return false;
		}
	}
	return false;
}

void ASMRoomPlayerState::OnTeamChanged(const ESMTeam PreviousTeam, const ESMTeam NewTeam)
{
	Super::OnTeamChanged(PreviousTeam, NewTeam);
}

bool ASMRoomPlayerState::CanChangeCharacterType(ESMCharacterType NewCharacterType) const
{
	return false;
}
