// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameState.h"

#include "GameFramework/GameMode.h"
#include "EngineUtils.h"
#include "FMODBlueprintStatics.h"
#include "Actors/Projectiles/Pool/SMProjectilePoolManagerComponent.h"
#include "Actors/Tiles/SMTile.h"
#include "Components/Core/SMRoundTimerComponent.h"
#include "Components/Core/SMTileManagerComponent.h"
#include "Net/UnrealNetwork.h"
#include "Utilities/SMLog.h"

ASMGameState::ASMGameState()
{
	RoundTimer = CreateDefaultSubobject<USMRoundTimerComponent>(TEXT("RoundTimer"));
	TileManager = CreateDefaultSubobject<USMTileManagerComponent>(TEXT("TileManager"));
	ProjectilePoolManager = CreateDefaultSubobject<USMProjectilePoolManagerComponent>(TEXT("ProjectilePoolManager"));

	TeamScores.Add(ESMTeam::EDM, 0);
	TeamScores.Add(ESMTeam::FutureBass, 0);

	TeamPhaseScores.Add(ESMTeam::None, 0);
	TeamPhaseScores.Add(ESMTeam::EDM, 0);
	TeamPhaseScores.Add(ESMTeam::FutureBass, 0);
}

void ASMGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASMGameState, ReplicatedEDMTeamScore);
	DOREPLIFETIME(ASMGameState, ReplicatedFutureBassTeamScore);
	DOREPLIFETIME(ASMGameState, ReplicatedEDMTeamPhaseScore);
	DOREPLIFETIME(ASMGameState, ReplicatedFutureBassTeamPhaseScore);
	DOREPLIFETIME(ASMGameState, ReplicatedCurrentMusicOwner)
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
			Tile->OnChangeTileWithTeamInformation.AddUObject(this, &ASMGameState::OnChangeTile);
		}

		NET_LOG(this, Warning, TEXT("현재 타일 개수: %d"), TotalTileCount)
	}
}

void ASMGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// 사운드 리소스 정리
	UFMODBlueprintStatics::EventInstanceStop(BackgroundMusicEventInstance, true);
}

void ASMGameState::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();

	if (HasAuthority())
	{
		RoundTimer->OnPreRoundTimeExpired.AddDynamic(this, &ThisClass::OnPreRoundTimeExpiredCallback);
		RoundTimer->OnRoundTimeExpired.AddDynamic(this, &ThisClass::OnRoundTimeExpiredCallback);
		RoundTimer->OnPostRoundTimeExpired.AddDynamic(this, &ThisClass::OnPostRoundTimeExpiredCallback);
		RoundTimer->StartTimer();
	}

	// 클라이언트만 배경음악 재생
	if (GetNetMode() == NM_Client)
	{
		NET_LOG(this, Verbose, TEXT("배경음악 재생"))
		PlayBackgroundMusic();
	}
}

void ASMGameState::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();

#if WITH_SERVER_CODE
	if (HasAuthority())
	{
		SetMusicOwner(ESMTeam::None);
	}
#endif
}

void ASMGameState::OnPreRoundTimeExpiredCallback()
{
	const UWorld* World = GetWorld();
	if (AGameMode* GameMode = World ? World->GetAuthGameMode<AGameMode>() : nullptr)
	{
		GameMode->StartMatch();
	}
}

void ASMGameState::OnRoundTimeExpiredCallback()
{
	const UWorld* World = GetWorld();
	if (AGameMode* GameMode = World ? World->GetAuthGameMode<AGameMode>() : nullptr)
	{
		GameMode->EndMatch();
	}

	EndRoundVictoryDefeatResult();
}

void ASMGameState::OnPostRoundTimeExpiredCallback()
{
	const UWorld* World = GetWorld();
	if (AGameMode* GameMode = World ? World->GetAuthGameMode<AGameMode>() : nullptr)
	{
		GameMode->StartToLeaveMap();
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

	if (GetMatchState() == MatchState::InProgress)
	{
		if (TeamScores[ESMTeam::FutureBass] == TeamScores[ESMTeam::EDM])
		{
			SetMusicOwner(ESMTeam::None);
		}
		else
		{
			const ESMTeam CurrentWinner = TeamScores[ESMTeam::EDM] > TeamScores[ESMTeam::FutureBass] ? ESMTeam::EDM : ESMTeam::FutureBass;
			if (GetCurrentMusicOwner() != CurrentWinner)
			{
				SetMusicOwner(CurrentWinner);
			}
		}
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

void ASMGameState::PlayBackgroundMusic()
{
	if (ensure(BackgroundMusic))
	{
		BackgroundMusicEventInstance = UFMODBlueprintStatics::PlayEvent2D(this, BackgroundMusic, true);
		UFMODBlueprintStatics::SetGlobalParameterByName("Winner", BGM_PARAMETER_NONE);
	}
}

void ASMGameState::SetMusicOwner(const ESMTeam InTeam)
{
#if WITH_SERVER_CODE
	if (HasAuthority())
	{
		NET_LOG(this, Verbose, TEXT("BGM 변경: %s"), *UEnum::GetValueAsString(InTeam))
		ReplicatedCurrentMusicOwner = InTeam;
	}
#endif
}

void ASMGameState::OnRep_ReplicatedCurrentMusicPlayer()
{
	if (BackgroundMusicEventInstance.Instance)
	{
		NET_LOG(this, Verbose, TEXT("서버로부터 BGM 변경됨: %s"), *UEnum::GetValueAsString(ReplicatedCurrentMusicOwner))
		switch (ReplicatedCurrentMusicOwner)
		{
			case ESMTeam::None:
				UFMODBlueprintStatics::SetGlobalParameterByName(TEXT("Winner"), BGM_PARAMETER_NONE);
				break;
			case ESMTeam::EDM:
				UFMODBlueprintStatics::SetGlobalParameterByName(TEXT("Winner"), BGM_PARAMETER_EDM);
				break;
			case ESMTeam::FutureBass:
				UFMODBlueprintStatics::SetGlobalParameterByName(TEXT("Winner"), BGM_PARAMETER_FUTURE_BASS);
				break;
		}
	}
}

void ASMGameState::MulticastRPCSendEndRoundResult_Implementation(ESMTeam VictoryTeam)
{
	NET_LOG(this, Log, TEXT("Round Victory Team: %s"), *UEnum::GetValueAsString(VictoryTeam))
	OnEndRound.Broadcast(VictoryTeam);
}
