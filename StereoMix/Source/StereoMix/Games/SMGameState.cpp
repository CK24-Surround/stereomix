// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameState.h"

#include "Net/UnrealNetwork.h"

ASMGameState::ASMGameState()
{
}

void ASMGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, RoundState);
}

void ASMGameState::SetRoundState(ESMRoundState NewRoundState)
{
	if (!HasAuthority())
	{
		return;
	}

	RoundState = NewRoundState;
	OnRep_RoundState();
}

void ASMGameState::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();

	if (HasAuthority())
	{
		SetRoundState(ESMRoundState::PreRound);
	}
}

void ASMGameState::OnRep_RoundState()
{
	switch (RoundState)
	{
		case ESMRoundState::PreRound:
		{
			OnPreRoundStart.Broadcast();
			break;
		}
		case ESMRoundState::InRound:
		{
			OnInRoundStart.Broadcast();
			break;
		}
		case ESMRoundState::PostRound:
		{
			OnPostRoundStart.Broadcast();
			break;
		}
		default:
		{
			break;
		}
	}
}
