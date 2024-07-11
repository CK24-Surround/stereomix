// Copyright Surround, Inc. All Rights Reserved.


#include "SMRoomState.h"

#include "Data/SMTeam.h"
#include "Games/SMCountdownTimerComponent.h"
#include "Net/UnrealNetwork.h"
#include "SMRoomPlayerState.h"
#include "Controllers/SMPlayerController.h"
#include "Utilities/SMLog.h"


ASMRoomState::ASMRoomState()
{
	CountdownTime = 5;
	CountdownTimer = CreateDefaultSubobject<USMCountdownTimerComponent>(TEXT("RoomCountdownTimer"));
}

void ASMRoomState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, MaxPlayersInGame)
	DOREPLIFETIME(ThisClass, MaxPlayersInTeam)
	DOREPLIFETIME(ThisClass, RoomName)
	DOREPLIFETIME(ThisClass, RoomCode)

	DOREPLIFETIME(ThisClass, TeamEdmPlayers)
	DOREPLIFETIME(ThisClass, TeamFutureBassPlayers)
}

void ASMRoomState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
}

void ASMRoomState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);

	if (HasAuthority())
	{
		if (ASMRoomPlayerState* RoomPlayerState = Cast<ASMRoomPlayerState>(PlayerState))
		{
			switch (RoomPlayerState->GetTeam())
			{
				case ESMTeam::EDM:
					TeamEdmPlayers.Remove(RoomPlayerState);
					OnTeamPlayersUpdated.Broadcast(ESMTeam::EDM);
					break;
				case ESMTeam::FutureBass:
					TeamFutureBassPlayers.Remove(RoomPlayerState);
					OnTeamPlayersUpdated.Broadcast(ESMTeam::FutureBass);
					break;
				default:
					break;
			}
		}
	}
}

TArray<ASMRoomPlayerState*> ASMRoomState::GetPlayersInTeam(const ESMTeam Team) const
{
	switch (Team)
	{
		case ESMTeam::EDM:
			return TeamEdmPlayers;

		case ESMTeam::FutureBass:
			return TeamFutureBassPlayers;

		default:
			return TArray<ASMRoomPlayerState*>();
	}
}

void ASMRoomState::NotifyPlayerJoined(ASMPlayerState* JoinedPlayer)
{
	if (OnPlayerJoined.IsBound())
	{
		OnPlayerJoined.Broadcast(JoinedPlayer);
	}
}

void ASMRoomState::NotifyPlayerLeft(ASMPlayerState* LeftPlayer)
{
	if (OnPlayerLeft.IsBound())
	{
		OnPlayerLeft.Broadcast(LeftPlayer);
	}
}

void ASMRoomState::NotifyPlayerTeamChanged(ASMPlayerState* Player, const ESMTeam PreviousTeam, const ESMTeam NewTeam)
{
	ASMRoomPlayerState* RoomPlayer = Cast<ASMRoomPlayerState>(Player);
	if (!RoomPlayer)
	{
		return;
	}

#if WITH_SERVER_CODE
	if (HasAuthority())
	{
		switch (PreviousTeam)
		{
			case ESMTeam::EDM:
				TeamEdmPlayers.Remove(RoomPlayer);
				OnTeamPlayersUpdated.Broadcast(ESMTeam::EDM);
				break;

			case ESMTeam::FutureBass:
				TeamFutureBassPlayers.Remove(RoomPlayer);
				OnTeamPlayersUpdated.Broadcast(ESMTeam::FutureBass);
				break;

			default:
				break;
		}

		switch (NewTeam)
		{
			case ESMTeam::EDM:
				TeamEdmPlayers.Add(RoomPlayer);
				OnTeamPlayersUpdated.Broadcast(ESMTeam::EDM);
				break;

			case ESMTeam::FutureBass:
				TeamFutureBassPlayers.Add(RoomPlayer);
				OnTeamPlayersUpdated.Broadcast(ESMTeam::FutureBass);
				break;

			default:
				break;
		}
	}
#endif
}

void ASMRoomState::OnRep_RoomName()
{
}

void ASMRoomState::OnRep_RoomCode()
{
}

void ASMRoomState::OnRep_TeamEdmPlayers()
{
	NET_LOG(this, Verbose, TEXT("[SMRoomState] EDM players updated: %d"), TeamEdmPlayers.Num())
	OnTeamPlayersUpdated.Broadcast(ESMTeam::EDM);
}

void ASMRoomState::OnRep_TeamFutureBassPlayers()
{
	NET_LOG(this, Verbose, TEXT("[SMRoomState] FutureBass players updated: %d"), TeamFutureBassPlayers.Num())
	OnTeamPlayersUpdated.Broadcast(ESMTeam::FutureBass);
}
