// Copyright Surround, Inc. All Rights Reserved.


#include "SMCharacterSelectState.h"

#include "Games/CountdownTimerComponent.h"
#include "Games/SMPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Utilities/SMLog.h"

ASMCharacterSelectState::ASMCharacterSelectState()
{
	CharacterSelectCountdownTime = 30;
	
	CurrentState = ECharacterSelectionStateType::Wait;
	CountdownTimer = CreateDefaultSubobject<UCountdownTimerComponent>(TEXT("RoomCountdownTimer"));
}

void ASMCharacterSelectState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
	if(HasAuthority())
	{
		// 캐릭터 선택 레벨에서 테스트하기 위해 임시로 팀 설정
		ASMPlayerState* SMPlayerState = Cast<ASMPlayerState>(PlayerState);
		if(SMPlayerState && SMPlayerState->GetTeam() == ESMTeam::None)
		{
			SMPlayerState->SetTeam(ESMTeam::FutureBass);
		}
	}
}

void ASMCharacterSelectState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);
}

void ASMCharacterSelectState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, CurrentState);
}

void ASMCharacterSelectState::SetCurrentState(ECharacterSelectionStateType NewState)
{
#if WITH_SERVER_CODE
	if (HasAuthority())
	{
		CurrentState = NewState;
		NET_LOG(this, Verbose, TEXT("Character selection state changed to %s"), *UEnum::GetValueAsString(NewState))
		OnCurrentStateChanged.Broadcast(CurrentState);
	}
#endif
}

void ASMCharacterSelectState::NotifyPlayerJoined(ASMPlayerState* JoinedPlayer)
{
	if (OnPlayerJoined.IsBound())
	{
		OnPlayerJoined.Broadcast(JoinedPlayer);
	}
}

void ASMCharacterSelectState::NotifyPlayerLeft(ASMPlayerState* LeftPlayer)
{
	if (OnPlayerLeft.IsBound())
	{
		OnPlayerLeft.Broadcast(LeftPlayer);
	}
}

void ASMCharacterSelectState::NotifyPlayerCharacterChanged(ASMPlayerState* Player, ESMCharacterType NewCharacter)
{
	if (OnPlayerCharacterChanged.IsBound())
	{
		OnPlayerCharacterChanged.Broadcast(Player, NewCharacter);
	}
}

void ASMCharacterSelectState::OnRep_CurrentState() const
{
	NET_LOG(this, Verbose, TEXT("OnRep_CurrentState"));
	OnCurrentStateChanged.Broadcast(CurrentState);
}