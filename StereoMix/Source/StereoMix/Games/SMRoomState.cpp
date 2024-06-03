// Copyright Surround, Inc. All Rights Reserved.


#include "SMRoomState.h"

#include "CountdownTimerComponent.h"
#include "Data/SMTeam.h"
#include "Net/UnrealNetwork.h"
#include "Player/SMRoomPlayerState.h"
#include "Utilities/SMLog.h"


ASMRoomState::ASMRoomState()
{
	ReplicatedMaxPlayersInGame = 6;
	ReplicatedMaxPlayersInTeam = 3;
	CountdownTime = 5;
	CountdownTimer = CreateDefaultSubobject<UCountdownTimerComponent>(TEXT("RoomCountdownTimer"));
}

void ASMRoomState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	// TODO: 지금 방법은 쓰레기 플레이어 (목록에 있는데 존재하지 않는 플레이어) 생길 수 있음. 새로운 방법을 찾아야 함.

	if (HasAuthority())
	{
		if (ASMRoomPlayerState* RoomPlayerState = Cast<ASMRoomPlayerState>(PlayerState))
		{
			UnreadyPlayers.Add(RoomPlayerState);
			OnTeamPlayersUpdated.Broadcast(ESMTeam::None);
		}
	}
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
			case ESMTeam::None:
				UnreadyPlayers.Remove(RoomPlayerState);
				OnTeamPlayersUpdated.Broadcast(ESMTeam::None);
				break;
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

void ASMRoomState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, UnreadyPlayers);
	DOREPLIFETIME(ThisClass, TeamEdmPlayers);
	DOREPLIFETIME(ThisClass, TeamFutureBassPlayers);

	DOREPLIFETIME(ThisClass, ReplicatedMaxPlayersInGame);
	DOREPLIFETIME(ThisClass, ReplicatedMaxPlayersInTeam);
	DOREPLIFETIME(ThisClass, ShortRoomCode);
}

TArray<ASMRoomPlayerState*> ASMRoomState::GetTeamPlayers(const ESMTeam Team) const
{
	switch (Team)
	{
	case ESMTeam::EDM:
		return TeamEdmPlayers;
	case ESMTeam::FutureBass:
		return TeamFutureBassPlayers;
	case ESMTeam::None:
	default:
		return UnreadyPlayers;
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
		case ESMTeam::None:
		default:
			UnreadyPlayers.Add(RoomPlayer);
			OnTeamPlayersUpdated.Broadcast(ESMTeam::None);
			break;
		}

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
		case ESMTeam::None:
		default:
			UnreadyPlayers.Remove(RoomPlayer);
			OnTeamPlayersUpdated.Broadcast(ESMTeam::None);
			break;;
		}
	}
#endif
}

void ASMRoomState::NotifyPlayerCharacterChanged(ASMPlayerState* Player, ESMCharacterType NewCharacter)
{
}

void ASMRoomState::OnRep_UnreadyPlayers() const
{
	NET_LOG(this, Verbose, TEXT("[SMRoomState] Unready players updated: %d"), UnreadyPlayers.Num())
	OnTeamPlayersUpdated.Broadcast(ESMTeam::None);
}

void ASMRoomState::OnRep_TeamEdmPlayers() const
{
	NET_LOG(this, Verbose, TEXT("[SMRoomState] EDM players updated: %d"), TeamEdmPlayers.Num())
	OnTeamPlayersUpdated.Broadcast(ESMTeam::EDM);
}

void ASMRoomState::OnRep_TeamFutureBassPlayers() const
{
	NET_LOG(this, Verbose, TEXT("[SMRoomState] FutureBass players updated: %d"), TeamFutureBassPlayers.Num())
	OnTeamPlayersUpdated.Broadcast(ESMTeam::FutureBass);
}
