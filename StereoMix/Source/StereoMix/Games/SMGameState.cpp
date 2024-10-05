// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameState.h"

#include "Actors/Projectiles/Pool/SMProjectilePoolManagerComponent.h"
#include "Components/Core/SMRoundTimerManagerComponent.h"
#include "Components/Core/SMScoreManagerComponent.h"
#include "Components/Core/SMScoreMusicManagerComponent.h"
#include "Components/Core/SMTileManagerComponent.h"
#include "Net/UnrealNetwork.h"
#include "Utilities/SMLog.h"

ASMGameState::ASMGameState()
{
	RoundTimerManager = CreateDefaultSubobject<USMRoundTimerManagerComponent>(TEXT("RoundTimer"));
	TileManager = CreateDefaultSubobject<USMTileManagerComponent>(TEXT("TileManager"));
	ScoreManager = CreateDefaultSubobject<USMScoreManagerComponent>(TEXT("ScoreManager"));
	ScoreMusicManager = CreateDefaultSubobject<USMScoreMusicManagerComponent>(TEXT("ScoreMusicManager"));
	ProjectilePoolManager = CreateDefaultSubobject<USMProjectilePoolManagerComponent>(TEXT("ProjectilePoolManager"));
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
