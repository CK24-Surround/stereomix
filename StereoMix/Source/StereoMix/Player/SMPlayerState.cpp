// Copyright Surround, Inc. All Rights Reserved.


#include "SMPlayerState.h"

#include "StereoMix.h"
#include "Games/SMGameState.h"
#include "Games/SMGameStateNotify.h"
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

	UE_LOG(LogStereoMix, Verbose, TEXT("[SMPlayerState] Player %s seamless traveled to %s"), *GetPlayerName(), *NewPlayerState->GetWorld()->GetMapName())

	if (ASMPlayerState* SMPlayerState = Cast<ASMPlayerState>(NewPlayerState))
	{
		SMPlayerState->Team = Team;
		SMPlayerState->CharacterType = CharacterType;
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
	// 플레이어가 레플리케이티드 변수까지 전부 초기화 된 이후에만 OnTeamChanged 호출 가능
	if (IsActorBeginningPlay() && !IsActorBeingDestroyed())
	{
		OnTeamChanged(PreviousTeam, Team);
	}
}

void ASMPlayerState::OnRep_CharacterType(const ESMCharacterType PreviousCharacterType)
{
	// 플레이어가 레플리케이티드 변수까지 전부 초기화 된 이후에만 OnCharacterTypeChanged 호출 가능
	if (IsActorBeginningPlay() && !IsActorBeingDestroyed())
	{
		OnCharacterTypeChanged(PreviousCharacterType, CharacterType);
	}
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

void ASMPlayerState::SetCharacterType_Implementation(ESMCharacterType NewCharacterType)
{
	if (!CanChangeCharacterType(NewCharacterType))
	{
		return;
	}
	const ESMCharacterType PreviousCharacterType = CharacterType;
	CharacterType = NewCharacterType;
	OnCharacterTypeChanged(PreviousCharacterType, NewCharacterType);
}

void ASMPlayerState::SetTeam_Implementation(const ESMTeam NewTeam)
{
	UE_LOG(LogStereoMix, Verbose, TEXT("[SMPlayerState] Player %s requested to change team to %s"), *GetPlayerName(), *UEnum::GetValueAsString(NewTeam))
	if (!CanChangeTeam(NewTeam))
	{
		return;
	}
	const ESMTeam PreviousTeam = Team;
	Team = NewTeam;
	OnTeamChanged(PreviousTeam, NewTeam);
}

bool ASMPlayerState::CanChangeTeam(const ESMTeam NewTeam) const
{
	return Team != NewTeam;
}

bool ASMPlayerState::CanChangeCharacterType(const ESMCharacterType NewCharacterType) const
{
	return CharacterType != NewCharacterType;
}
