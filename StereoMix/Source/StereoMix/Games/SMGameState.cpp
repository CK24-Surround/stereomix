// Fill out your copyright notice in the Description page of Project Settings.


#include "SMGameState.h"

#include "EngineUtils.h"
#include "SMGameMode.h"
#include "Data/SMDesignData.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Tiles/SMTile.h"
#include "Utilities/SMLog.h"

ASMGameState::ASMGameState()
{
	TeamScores.Add(ESMTeam::EDM, 0);
	TeamScores.Add(ESMTeam::FutureBass, 0);

	TeamPhaseScores.Add(ESMTeam::None, 0);
	TeamPhaseScores.Add(ESMTeam::EDM, 0);
	TeamPhaseScores.Add(ESMTeam::FutureBass, 0);
}

void ASMGameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (HasAuthority())
	{
		for (ASMTile* Tile : TActorRange<ASMTile>(GetWorld()))
		{
			if (!Tile)
			{
				continue;
			}

			++TotalTileCount;
			Tile->OnChangeTile.AddUObject(this, &ASMGameState::OnChangeTile);
		}

		NET_LOG(this, Warning, TEXT("현재 타일 개수: %d"), TotalTileCount);
	}
}

void ASMGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASMGameState, ReplicatedRemainRoundTime);
	DOREPLIFETIME(ASMGameState, ReplicatedEDMTeamScore);
	DOREPLIFETIME(ASMGameState, ReplicatedFutureBassTeamScore);
	DOREPLIFETIME(ASMGameState, ReplicatedRemainPhaseTime);
	DOREPLIFETIME(ASMGameState, ReplicatedPhaseTime);
	DOREPLIFETIME(ASMGameState, ReplicatedCurrentPhaseNumber);
	DOREPLIFETIME(ASMGameState, ReplicatedEDMTeamPhaseScore);
	DOREPLIFETIME(ASMGameState, ReplicatedFutureBassTeamPhaseScore);
}

void ASMGameState::OnRep_ReplicatedRemainRoundTime()
{
	Super::BeginPlay();

	(void)OnChangeRoundTime.ExecuteIfBound(ReplicatedRemainRoundTime);
}

void ASMGameState::SetTeamScores(ESMTeam InTeam, int32 InScore)
{
	if (!HasAuthority())
	{
		return;
	}

	TeamScores[InTeam] = InScore;

	// 클라이언트에서 점수 정보를 받을 수 있도록 리플리케이션 되는 변수에 할당합니다.
	ReplicatedEDMTeamScore = TeamScores[ESMTeam::EDM];
	ReplicatedFutureBassTeamScore = TeamScores[ESMTeam::FutureBass];
}

void ASMGameState::SetTeamPhaseScores(ESMTeam InTeam, int32 InScore)
{
	if (!HasAuthority())
	{
		return;
	}

	TeamPhaseScores[InTeam] = InScore;

	// 클라이언트에서 점수 정보를 받을 수 있도록 리플리케이션 되는 변수에 할당합니다.
	ReplicatedEDMTeamPhaseScore = TeamPhaseScores[ESMTeam::EDM];
	ReplicatedFutureBassTeamPhaseScore = TeamPhaseScores[ESMTeam::FutureBass];
}

void ASMGameState::OnChangeTile(ESMTeam PreviousTeam, ESMTeam NewTeam)
{
	// 이미 타일 측에서 팀이 변경되지 않은 경우 처리되지 않도록 예외처리를 진행해두었지만 만약을 위한 방어코드입니다.
	if (PreviousTeam == NewTeam)
	{
		return;
	}

	// TODO: 나중에 중립 타일화 시키는 기능을 구현해야합니다.
	if (NewTeam == ESMTeam::None)
	{
		return;
	}

	if (PreviousTeam == ESMTeam::None)
	{
		// 중립 타일을 점령하는 경우 스코어에 그대로 더해줍니다.
		AddTeamScore(NewTeam);
	}
	else
	{
		// 이미 다른팀이 점령한 타일을 점령하는 경우 스코어를 스왑해줍니다.
		SwapScore(PreviousTeam, NewTeam);
	}
}

void ASMGameState::AddTeamScore(ESMTeam InTeam)
{
	SetTeamScores(InTeam, TeamScores[InTeam] + 1);
}

void ASMGameState::SwapScore(ESMTeam PreviousTeam, ESMTeam NewTeam)
{
	SetTeamScores(PreviousTeam, TeamScores[PreviousTeam] - 1);
	SetTeamScores(NewTeam, TeamScores[NewTeam] + 1);
}

void ASMGameState::OnRep_ReplicatedEDMTeamScore()
{
	(void)OnChangeEDMTeamScore.ExecuteIfBound(ReplicatedEDMTeamScore);
}

void ASMGameState::OnRep_ReplicatedFutureBassTeamScore()
{
	(void)OnChangeFutureBassTeamScore.ExecuteIfBound(ReplicatedFutureBassTeamScore);
}

void ASMGameState::OnRep_ReplicatedEDMTeamPhaseScore()
{
	(void)OnChangeEDMTeamPhaseScore.ExecuteIfBound(ReplicatedEDMTeamPhaseScore);
}

void ASMGameState::OnRep_ReplicatedFutureBassTeamPhaseScore()
{
	(void)OnChangeFutureBassTeamPhaseScore.ExecuteIfBound(ReplicatedFutureBassTeamPhaseScore);
}

void ASMGameState::EndPhase()
{
	const ESMTeam VictoryTeam = CalculateVictoryTeam();
	SetTeamPhaseScores(VictoryTeam, TeamPhaseScores[VictoryTeam] + 1);
}

void ASMGameState::OnRep_ReplicatedRemainPhaseTime()
{
	(void)OnChangePhaseTime.ExecuteIfBound(ReplicatedRemainPhaseTime, ReplicatedPhaseTime);
}

void ASMGameState::OnRep_ReplicatedCurrentPhaseNumber()
{
	(void)OnChangePhase.ExecuteIfBound(ReplicatedCurrentPhaseNumber);
}

ESMTeam ASMGameState::CalculateVictoryTeam()
{
	// 무승부면 None팀이 승리 팀으로 전달됩니다.
	ESMTeam VictoryTeam = ESMTeam::None;
	if (TeamScores[ESMTeam::EDM] != TeamScores[ESMTeam::FutureBass])
	{
		VictoryTeam = TeamScores[ESMTeam::EDM] > TeamScores[ESMTeam::FutureBass] ? ESMTeam::EDM : ESMTeam::FutureBass;
	}

	return VictoryTeam;
}

void ASMGameState::EndRoundVictoryDefeatResult()
{
	MulticastRPCSendEndRoundResult(CalculateVictoryTeam());
}

void ASMGameState::MulticastRPCSendEndRoundResult_Implementation(ESMTeam VictoryTeam)
{
	OnEndRound.Broadcast(VictoryTeam);
}
