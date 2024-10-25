// Copyright Surround, Inc. All Rights Reserved.


#include "SMPlayerState.h"

#include "Games/SMGameState.h"
#include "Games/SMGameStateNotify.h"
#include "Net/UnrealNetwork.h"
#include "StereoMix.h"
#include "StereoMixLog.h"
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

	UE_LOG(LogStereoMix, Verbose, TEXT("[SMPlayerState] Player %s seamless traveled to %s"), *GetPlayerName(), *NewPlayerState->GetWorld()->GetMapName())

	// TODO: CopyProperties로 코드 옮기기
	if (ASMPlayerState* SMPlayerState = Cast<ASMPlayerState>(NewPlayerState))
	{
		if (SMPlayerState->bCopyTeamOnSeamlessTravel)
		{
			UE_LOG(LogStereoMix, Verbose, TEXT("[SMPlayerState] Copy team: %s -> %s"), *UEnum::GetValueAsString(Team), *UEnum::GetValueAsString(SMPlayerState->Team))
			SMPlayerState->Team = Team;
		}
		if (SMPlayerState->bCopyCharacterTypeOnSeamlessTravel)
		{
			UE_LOG(LogStereoMix, Verbose, TEXT("[SMPlayerState] Copy character type: %s -> %s"), *UEnum::GetValueAsString(CharacterType), *UEnum::GetValueAsString(SMPlayerState->CharacterType))
			SMPlayerState->CharacterType = CharacterType;
		}
	}
}

void ASMPlayerState::BeginPlay()
{
	Super::BeginPlay();
	if (ISMGameStateNotify* GameStateNotify = Cast<ISMGameStateNotify>(GetWorld()->GetGameState()))
	{
		GameStateNotify->NotifyPlayerJoined(this);
	}
}

void ASMPlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (GetWorld())
	{
		if (ISMGameStateNotify* GameStateNotify = Cast<ISMGameStateNotify>(GetWorld()->GetGameState()))
		{
			GameStateNotify->NotifyPlayerLeft(this);
		}
	}
}

void ASMPlayerState::OnRep_Team(const ESMTeam PreviousTeam)
{
	OnTeamChanged(PreviousTeam, Team);
}

void ASMPlayerState::OnRep_CharacterType(const ESMCharacterType PreviousCharacterType)
{
	NET_LOG(this, Verbose, TEXT("[SMPlayerState] Player %s character type replicated: %s -> %s"), *GetPlayerName(), *UEnum::GetValueAsString(PreviousCharacterType), *UEnum::GetValueAsString(CharacterType))
	OnCharacterTypeChanged(PreviousCharacterType, CharacterType);
}

void ASMPlayerState::OnTeamChanged(const ESMTeam PreviousTeam, const ESMTeam NewTeam)
{
	NET_LOG(this, Verbose, TEXT("[SMPlayerState] Player %s changed team: %s -> %s"), *GetPlayerName(), *UEnum::GetValueAsString(PreviousTeam), *UEnum::GetValueAsString(NewTeam))
	if (TeamChangedEvent.IsBound())
	{
		TeamChangedEvent.Broadcast(NewTeam);
	}

	if (GetWorld())
	{
		if (ISMGameStateNotify* GameStateNotify = Cast<ISMGameStateNotify>(GetWorld()->GetGameState()))
		{
			GameStateNotify->NotifyPlayerTeamChanged(this, PreviousTeam, NewTeam);
		}
	}
}

void ASMPlayerState::OnCharacterTypeChanged(const ESMCharacterType PreviousCharacterType, const ESMCharacterType NewCharacterType)
{
	NET_LOG(this, Verbose, TEXT("[SMPlayerState] Player %s changed character type: %s -> %s"), *GetPlayerName(), *UEnum::GetValueAsString(PreviousCharacterType), *UEnum::GetValueAsString(NewCharacterType))
	if (CharacterTypeChangedEvent.IsBound())
	{
		CharacterTypeChangedEvent.Broadcast(NewCharacterType);
	}

	if (GetWorld())
	{
		if (ISMGameStateNotify* GameStateNotify = Cast<ISMGameStateNotify>(GetWorld()->GetGameState()))
		{
			GameStateNotify->NotifyPlayerCharacterChanged(this, NewCharacterType);
		}
	}
}

bool ASMPlayerState::SetCharacterType(ESMCharacterType NewCharacterType)
{
	// !CanChangeCharacterType(NewCharacterType) 왜 안되는지 확인 필요...
	if (CharacterType == NewCharacterType)
	{
		return false;
	}
	const ESMCharacterType PreviousCharacterType = CharacterType;
	CharacterType = NewCharacterType;
	OnCharacterTypeChanged(PreviousCharacterType, NewCharacterType);
	return true;
}

bool ASMPlayerState::SetTeam(const ESMTeam NewTeam)
{
	if (!CanChangeTeam(NewTeam))
	{
		return false;
	}
	const ESMTeam PreviousTeam = Team;
	Team = NewTeam;
	OnTeamChanged(PreviousTeam, NewTeam);
	return true;
}

bool ASMPlayerState::CanChangeTeam(const ESMTeam NewTeam) const
{
	return Team != NewTeam;
}

bool ASMPlayerState::CanChangeCharacterType(const ESMCharacterType NewCharacterType) const
{
	return CharacterType != NewCharacterType;
}
