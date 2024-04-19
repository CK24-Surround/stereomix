// Fill out your copyright notice in the Description page of Project Settings.


#include "SMGameState.h"

#include "EngineUtils.h"
#include "Data/SMDesignData.h"
#include "Net/UnrealNetwork.h"
#include "Tiles/SMTile.h"
#include "Utilities/SMAssetPath.h"
#include "Utilities/SMLog.h"

ASMGameState::ASMGameState()
{
	static ConstructorHelpers::FObjectFinder<USMDesignData> DesignDataRef(SMAssetPath::DesignData);
	if (DesignDataRef.Object)
	{
		DesignData = DesignDataRef.Object;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("DesignData 로드에 실패했습니다."));
	}

	TeamScores.Add(ESMTeam::FutureBass, 0);
	TeamScores.Add(ESMTeam::EDM, 0);

	ReplicatedEDMTeamScore = 0;
	ReplicatedFutureBaseTeamScore = 0;
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

		RoundTime = DesignData->RoundTime;
		RemainRoundTime = RoundTime;
	}
}

void ASMGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASMGameState, RemainRoundTime);
	DOREPLIFETIME(ASMGameState, ReplicatedEDMTeamScore);
	DOREPLIFETIME(ASMGameState, ReplicatedFutureBaseTeamScore);
}

void ASMGameState::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		const float OneSecond = GetWorldSettings()->GetEffectiveTimeDilation();
		GetWorld()->GetTimerManager().SetTimer(RoundTimerHandle, this, &ASMGameState::PerformRoundTime, OneSecond, true);
	}
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
	ReplicatedFutureBaseTeamScore = TeamScores[ESMTeam::FutureBass];
}

void ASMGameState::OnChangeTile(ESMTeam PreviousTeam, ESMTeam NewTeam)
{
	// 이런 경우는 없어야하지만 만약을 위한 방어 코드입니다.
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
		AddTeamScore(NewTeam);
	}
	else
	{
		SwapScore(PreviousTeam, NewTeam);
	}

	PrintScore();
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

void ASMGameState::OnRep_ReplicatedFutureBaseTeamScore()
{
	(void)OnChangeFutureBaseTeamScore.ExecuteIfBound(ReplicatedFutureBaseTeamScore);
}

void ASMGameState::PrintScore()
{
	NET_LOG(this, Log, TEXT("FutureBase팀 스코어: %d EDM팀 스코어: %d"), TeamScores[ESMTeam::FutureBass], TeamScores[ESMTeam::EDM]);
}

void ASMGameState::SetRemainRoundTime(int32 InRemainRoundTime)
{
	RemainRoundTime = InRemainRoundTime;
}

void ASMGameState::PerformRoundTime()
{
	SetRemainRoundTime(RemainRoundTime - 1);
}

void ASMGameState::OnRep_RemainRoundTime()
{
	(void)OnChangeRoundTime.ExecuteIfBound(RemainRoundTime);
}
