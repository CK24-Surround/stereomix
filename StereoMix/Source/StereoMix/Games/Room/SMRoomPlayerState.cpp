// Copyright Surround, Inc. All Rights Reserved.


#include "SMRoomPlayerState.h"

#include "Net/UnrealNetwork.h"


ASMRoomPlayerState::ASMRoomPlayerState()
{
	CurrentState = ERoomPlayerStateType::Loading;

	bCopyTeamOnSeamlessTravel = false;
	bCopyCharacterTypeOnSeamlessTravel = false;
}

void ASMRoomPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentState);
}

void ASMRoomPlayerState::BeginPlay()
{
	Super::BeginPlay();
	RoomState = GetWorld()->GetGameStateChecked<ASMRoomState>();

#if WITH_SERVER_CODE
	if (HasAuthority())
	{
		CurrentState = ERoomPlayerStateType::Unready;
	}
#endif
}

void ASMRoomPlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ASMRoomPlayerState::ResponseChangeTeam_Implementation(bool bSuccess, ESMTeam NewTeam)
{
	if (OnTeamChangeResponse.IsBound())
	{
		OnTeamChangeResponse.Broadcast(bSuccess, NewTeam);
	}
}

void ASMRoomPlayerState::ChangeTeam_Implementation(ESMTeam NewTeam)
{
	if (SetTeam(NewTeam))
	{
		ResponseChangeTeam(true, NewTeam);
	}
	else
	{
		ResponseChangeTeam(false, NewTeam);
	}
}

void ASMRoomPlayerState::SetCurrentState_Implementation(const ERoomPlayerStateType NewState)
{
	CurrentState = NewState;
	if (OnCurrentStateChanged.IsBound())
	{
		OnCurrentStateChanged.Broadcast(CurrentState);
	}
}

void ASMRoomPlayerState::OnRep_CurrentState()
{
	if (OnCurrentStateChanged.IsBound())
	{
		OnCurrentStateChanged.Broadcast(CurrentState);
	}
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
				return RoomState->GetTeamEdmPlayers().Num() < RoomState->MaxPlayersInTeam;
			case ESMTeam::FutureBass:
				return RoomState->GetTeamFutureBassPlayers().Num() < RoomState->MaxPlayersInTeam;
			default:
				return false;
		}
	}
	return false;
}

void ASMRoomPlayerState::OnTeamChanged(const ESMTeam PreviousTeam, const ESMTeam NewTeam)
{
	Super::OnTeamChanged(PreviousTeam, NewTeam);

#if WITH_SERVER_CODE
	if (HasAuthority())
	{
		switch (NewTeam)
		{
			case ESMTeam::EDM:
			case ESMTeam::FutureBass:
				CurrentState = ERoomPlayerStateType::Ready;
				break;

			case ESMTeam::None:
			default:
				CurrentState = ERoomPlayerStateType::Unready;
				break;
		}
	}
#endif
}
