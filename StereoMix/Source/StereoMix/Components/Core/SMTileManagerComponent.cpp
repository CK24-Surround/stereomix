// Copyright Surround, Inc. All Rights Reserved.


#include "SMTileManagerComponent.h"

#include "GameFramework/GameMode.h"
#include "EngineUtils.h"
#include "SMScoreMusicManagerComponent.h"
#include "Actors/Tiles/SMTile.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"
#include "FunctionLibraries/SMTileFunctionLibrary.h"
#include "Games/SMGameState.h"
#include "Net/UnrealNetwork.h"
#include "Utilities/SMLog.h"


USMTileManagerComponent::USMTileManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;

	SetIsReplicatedByDefault(true);

	for (int32 i = 1; i < static_cast<int32>(ESMTeam::Max); ++i)
	{
		const ESMTeam Team = static_cast<ESMTeam>(i);
		TileScores.Add(Team, 0);
	}
}

void USMTileManagerComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EDMTileScore);
	DOREPLIFETIME(ThisClass, FBTileScore);
}

void USMTileManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	const UWorld* World = GetWorld();
	if ((GetOwnerRole() == ROLE_Authority) && World)
	{
		int32 TotalTileCount = 0;
		for (ASMTile* Tile : TActorRange<ASMTile>(World))
		{
			if (Tile)
			{
				++TotalTileCount;
				Tile->OnChangeTileWithTeamInformation.AddUObject(this, &ThisClass::OnTileChanged);
			}
		}

		NET_LOG(GetOwner(), Warning, TEXT("현재 타일 개수: %d"), TotalTileCount)
	}
}

void USMTileManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	const AActor* CachedOwner = GetOwner();
	CachedScoreMusicManager = CachedOwner ? CachedOwner->FindComponentByClass<USMScoreMusicManagerComponent>() : nullptr;
}

TArray<ASMTile*> USMTileManagerComponent::CaptureTiles(const TArray<ASMTile*>& TilesToBeCaptured, const AActor* Instigator, const TOptional<ESMTeam>& OverrideTeamOption)
{
	TArray<ASMTile*> CapturedTiles;
	const ESMTeam InstigatorTeam = OverrideTeamOption.Get(USMTeamBlueprintLibrary::GetTeam(Instigator));

	const FColor DebugColor = FColor::MakeRandomColor();
	for (ASMTile* TileToBeCaptured : TilesToBeCaptured)
	{
		const ESMTeam TileTeam = TileToBeCaptured ? TileToBeCaptured->GetTeam() : ESMTeam::None;
		if (!TileToBeCaptured || InstigatorTeam == TileTeam)
		{
			continue;
		}

		TileToBeCaptured->TileTrigger(InstigatorTeam);
		CapturedTiles.Add(TileToBeCaptured);

		if (bShowDebug)
		{
			DrawDebugBox(GetWorld(), TileToBeCaptured->GetTileLocation(), FVector(USMTileFunctionLibrary::DefaultTileSize / 2.0), DebugColor, false, 1.5f);
		}
	}

	if (CapturedTiles.Num() > 0)
	{
		OnTilesCaptured.Broadcast(Instigator, CapturedTiles.Num());
	}

	return CapturedTiles;
}

void USMTileManagerComponent::SetTileScores(ESMTeam Team, int32 Score)
{
	TileScores[Team] = Score;

	EDMTileScore = TileScores[ESMTeam::EDM];
	FBTileScore = TileScores[ESMTeam::FutureBass];

	OnRep_EDMTileScore();
	OnRep_FBTileScore();
}

void USMTileManagerComponent::ShowGameResult()
{
	MulticastSendGameResult(CalculateVictoryTeam());
}

void USMTileManagerComponent::ResetTileScores()
{
	for (int32 TeamAsInt32 = 1; TeamAsInt32 < static_cast<int32>(ESMTeam::Max); ++TeamAsInt32)
	{
		SetTileScores(static_cast<ESMTeam>(TeamAsInt32), 0);
	}
}

void USMTileManagerComponent::OnTileChanged(ESMTeam PreviousTeam, ESMTeam NewTeam)
{
	if (PreviousTeam == NewTeam) // 이미 타일 측에서 팀이 변경되지 않은 경우 처리되지 않도록 예외처리를 진행해두었지만 만약을 위한 방어코드입니다.
	{
		return;
	}

	if (NewTeam == ESMTeam::None) // None 팀으로 점령하는 경우 타일이 다시 중립화 됩니다.
	{
		NeutralizeTile(PreviousTeam);
	}
	else
	{
		if (PreviousTeam == ESMTeam::None) // 중립 타일을 점령하는 경우 스코어에 그대로 더해줍니다.
		{
			AddTileScore(NewTeam);
		}
		else // 이미 다른팀이 점령한 타일을 점령하는 경우 스코어를 스왑해줍니다.
		{
			SwapTileScore(PreviousTeam, NewTeam);
		}
	}

	ChangeScoreMusic();
}

void USMTileManagerComponent::ChangeScoreMusic()
{
	const UWorld* World = GetWorld();
	const ASMGameState* GameState = World ? World->GetGameState<ASMGameState>() : nullptr;
	const FName MatchState = GameState ? GameState->GetMatchState() : NAME_None;
	if (CachedScoreMusicManager.IsValid() && (MatchState == MatchState::InProgress))
	{
		if (TileScores[ESMTeam::FutureBass] == TileScores[ESMTeam::EDM])
		{
			CachedScoreMusicManager->SetScoreMusicPlayingTeam(ESMTeam::None);
		}
		else
		{
			const ESMTeam CurrentWinner = TileScores[ESMTeam::EDM] > TileScores[ESMTeam::FutureBass] ? ESMTeam::EDM : ESMTeam::FutureBass;
			if (CachedScoreMusicManager->GetScoreMusicPlayingTeam() != CurrentWinner)
			{
				CachedScoreMusicManager->SetScoreMusicPlayingTeam(CurrentWinner);
			}
		}
	}
}

void USMTileManagerComponent::NeutralizeTile(ESMTeam PreviousTeam)
{
	SetTileScores(PreviousTeam, TileScores[PreviousTeam] - 1);
}

void USMTileManagerComponent::AddTileScore(ESMTeam Team)
{
	SetTileScores(Team, TileScores[Team] + 1);
}

void USMTileManagerComponent::SwapTileScore(ESMTeam PreviousTeam, ESMTeam NewTeam)
{
	SetTileScores(PreviousTeam, TileScores[PreviousTeam] - 1);
	SetTileScores(NewTeam, TileScores[NewTeam] + 1);
}

void USMTileManagerComponent::OnRep_EDMTileScore()
{
	TileScores[ESMTeam::EDM] = EDMTileScore;
	OnEDMTileScoreChanged.Broadcast(EDMTileScore);
}

void USMTileManagerComponent::OnRep_FBTileScore()
{
	TileScores[ESMTeam::FutureBass] = FBTileScore;
	OnFBTileScoreChanged.Broadcast(FBTileScore);
}

ESMTeam USMTileManagerComponent::CalculateVictoryTeam()
{
	ESMTeam VictoryTeam = ESMTeam::None;
	if (TileScores[ESMTeam::EDM] != TileScores[ESMTeam::FutureBass])
	{
		VictoryTeam = TileScores[ESMTeam::EDM] > TileScores[ESMTeam::FutureBass] ? ESMTeam::EDM : ESMTeam::FutureBass;
	}

	return VictoryTeam;
}

void USMTileManagerComponent::MulticastSendGameResult_Implementation(ESMTeam VictoryTeam)
{
	OnVictoryTeamAnnounced.Broadcast(VictoryTeam);
}
