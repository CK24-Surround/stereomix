// Copyright Surround, Inc. All Rights Reserved.


#include "SMRoomState.h"
#include "StereoMix.h"
#include "Net/UnrealNetwork.h"
#include "Player/SMRoomPlayerState.h"
#include "Utilities/SMLog.h"

ASMRoomState::ASMRoomState()
{
	MaxPlayersInTeam = 3;
}

void ASMRoomState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Countdown);
}

int32 ASMRoomState::GetPlayerCountInTeam(const ESMTeam Team) const
{
	int32 Count = 0;
	for (APlayerState* PlayerState : PlayerArray)
	{
		if (const ASMRoomPlayerState* RoomPlayerState = Cast<ASMRoomPlayerState>(PlayerState); RoomPlayerState && RoomPlayerState->GetTeam() == Team)
		{
			++Count;
		}
	}
	return Count;
}

void ASMRoomState::BroadcastPlayerAdded(APlayerState* PlayerState) const
{
	// NET_LOG(this, Verbose, TEXT("Player %s added to room with team %s"), *PlayerState->GetPlayerName(), *UEnum::GetValueAsString(Cast<ASMRoomPlayerState>(PlayerState)->GetTeam()))
	PlayerAddEvent.Broadcast(PlayerState);
}

void ASMRoomState::BroadcastPlayerRemoved(APlayerState* PlayerState) const
{
	// NET_LOG(this, Verbose, TEXT("Player %s removed from room"), *PlayerState->GetPlayerName())
 	PlayerRemoveEvent.Broadcast(PlayerState);
}

void ASMRoomState::BroadcastTeamChanged(ASMRoomPlayerState* TargetPlayerState, const ESMTeam NewTeam) const
{
	PlayerTeamChangeEvent.Broadcast(TargetPlayerState, NewTeam);
}

void ASMRoomState::OnRep_Countdown() const
{
	NET_LOG(this, Verbose, TEXT("Countdown: %d"), Countdown)
}

void ASMRoomState::StartCountdown_Implementation()
{
}

void ASMRoomState::CancelCountdown_Implementation()
{
}
