// Copyright Surround, Inc. All Rights Reserved.


#include "SMRoomPlayerController.h"

#include "Utilities/SMLog.h"

ASMRoomPlayerController::ASMRoomPlayerController()
{
}

void ASMRoomPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	RoomPlayerState = Cast<ASMPlayerState>(PlayerState);
	if (RoomPlayerState.IsValid())
	{
		RoomPlayerState->TeamChangedEvent.AddDynamic(this, &ASMRoomPlayerController::OnTeamChanged);
	}
}

void ASMRoomPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ASMRoomPlayerController::ChangeTeam_Implementation(ESMTeam NewTeam)
{
#if WITH_SERVER_CODE
	if (RoomPlayerState.IsValid())
	{
		RoomPlayerState->SetTeam(NewTeam);
	}
#endif
}

void ASMRoomPlayerController::OnTeamChanged(ESMTeam NewTeam)
{
	NET_LOG(this, Verbose, TEXT("[SMRoomPlayerController] ChangeTeam: %s"), *UEnum::GetValueAsString(NewTeam))
	TeamChangedEvent.Broadcast(NewTeam);
}

