// Copyright Surround, Inc. All Rights Reserved.


#include "SMPlayerState.h"

#include "Games/SMGameState.h"
#include "Net/UnrealNetwork.h"
#include "Utilities/SMLog.h"

void ASMPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, Team);
	DOREPLIFETIME(ThisClass, CharacterType);
}

void ASMPlayerState::SeamlessTravelTo(APlayerState* NewPlayerState)
{
	Super::SeamlessTravelTo(NewPlayerState);

	if (ASMPlayerState* SMPlayerState = Cast<ASMPlayerState>(NewPlayerState))
	{
		SMPlayerState->Team = Team;
		SMPlayerState->CharacterType = CharacterType;
	}
}

void ASMPlayerState::SetTeam_Implementation(const ESMTeam NewTeam)
{
#if WITH_SERVER_CODE
	if (CanChangeTeam(NewTeam))
	{
		const ESMTeam PreviousTeam = Team;
		Team = NewTeam;
		OnTeamChanged(PreviousTeam, NewTeam);
	}
#endif
}

void ASMPlayerState::SetCharacterType_Implementation(const ESMCharacterType NewCharacterType)
{
#if  WITH_SERVER_CODE
	if (CanChangeCharacterType(NewCharacterType))
	{
		const ESMCharacterType PreviousCharacterType = CharacterType;
		CharacterType = NewCharacterType;
		OnCharacterTypeChanged(PreviousCharacterType, CharacterType);
	}
#endif
}

void ASMPlayerState::OnTeamChanged(const ESMTeam PreviousTeam, const ESMTeam NewTeam)
{
	if (PreviousTeam != ESMTeam::None && NewTeam != PreviousTeam)
	{
		NET_LOG(this, Verbose, TEXT("[PlayerState] Player %s changed team %s to %s"), *GetName(), *UEnum::GetValueAsString(PreviousTeam), *UEnum::GetValueAsString(NewTeam))
		TeamChangedEvent.Broadcast(NewTeam);
	}
}

void ASMPlayerState::OnCharacterTypeChanged(const ESMCharacterType PreviousCharacterType, const ESMCharacterType NewCharacterType)
{
	NET_LOG(this, Verbose, TEXT("[PlayerState] Player %s changed character type %s to %s"), *GetName(), *UEnum::GetValueAsString(PreviousCharacterType), *UEnum::GetValueAsString(NewCharacterType))
	CharacterTypeChangedEvent.Broadcast(NewCharacterType);
}

void ASMPlayerState::OnRep_Team(const ESMTeam PreviousTeam)
{
	OnTeamChanged(PreviousTeam, Team);
}

void ASMPlayerState::OnRep_CharacterType(const ESMCharacterType PreviousCharacterType)
{
	OnCharacterTypeChanged(PreviousCharacterType, CharacterType);
}

bool ASMPlayerState::CanChangeTeam(const ESMTeam NewTeam) const
{
	return Team != NewTeam;
}

bool ASMPlayerState::CanChangeCharacterType(const ESMCharacterType NewCharacterType) const
{
	return CharacterType != NewCharacterType;
}
