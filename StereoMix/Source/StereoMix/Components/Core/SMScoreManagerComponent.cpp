// Copyright Studio Surround. All Rights Reserved.


#include "SMScoreManagerComponent.h"

#include "GameFramework/GameStateBase.h"
#include "SMTileManagerComponent.h"
#include "StereoMixLog.h"


void USMScoreManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	UWorld* World = GetWorld();
	AGameStateBase* GameState = World->GetGameState();
	if (!World || !GameState)
	{
		return;
	}

	USMTileManagerComponent* TileManager = GameState->GetComponentByClass<USMTileManagerComponent>();
	if (!TileManager)
	{
		return;
	}

	TileManager->OnTilesCaptured.AddDynamic(this, &ThisClass::AddTotalCapturedTiles);
}

void USMScoreManagerComponent::AddTotalCapturedTiles(const AActor* CapturedInstigator, int32 CapturedTilesCount)
{
	if (!CapturedInstigator)
	{
		return;
	}

	if (PlayerScoreData.Contains(CapturedInstigator))
	{
		PlayerScoreData[CapturedInstigator].TotalCapturedTiles += CapturedTilesCount;
		UE_LOG(LogStereoMix, Warning, TEXT("PlayerScoreData contains CapturedInstigator: %d"), PlayerScoreData[CapturedInstigator].TotalCapturedTiles);
		return;
	}

	UE_LOG(LogStereoMix, Warning, TEXT("PlayerScoreData does not contain CapturedInstigator"));
	FPlayerScoreData NewPlayerScoreData;
	NewPlayerScoreData.TotalCapturedTiles = CapturedTilesCount;
	PlayerScoreData.Add(CapturedInstigator, NewPlayerScoreData);
}

void USMScoreManagerComponent::AddTotalDamageDealt(const AActor* TargetPlayer, int32 DamageDealt)
{
	if (!TargetPlayer)
	{
		return;
	}

	if (PlayerScoreData.Contains(TargetPlayer))
	{
		PlayerScoreData[TargetPlayer].TotalDamageDealt += DamageDealt;
		return;
	}

	FPlayerScoreData NewPlayerScoreData;
	NewPlayerScoreData.TotalDamageDealt = DamageDealt;
	PlayerScoreData.Add(TargetPlayer, NewPlayerScoreData);
}

void USMScoreManagerComponent::AddTotalNeutralizes(const AActor* TargetPlayer, int32 Neutralizes)
{
	if (!TargetPlayer)
	{
		return;
	}

	if (PlayerScoreData.Contains(TargetPlayer))
	{
		PlayerScoreData[TargetPlayer].TotalNeutralizes += Neutralizes;
		return;
	}

	FPlayerScoreData NewPlayerScoreData;
	NewPlayerScoreData.TotalNeutralizes = Neutralizes;
	PlayerScoreData.Add(TargetPlayer, NewPlayerScoreData);
}

void USMScoreManagerComponent::AddTotalNoiseBreakUsage(const AActor* TargetPlayer, int32 NoiseBreakUsage)
{
	if (!TargetPlayer)
	{
		return;
	}

	if (PlayerScoreData.Contains(TargetPlayer))
	{
		PlayerScoreData[TargetPlayer].TotalNoiseBreakUsage += NoiseBreakUsage;
		return;
	}

	FPlayerScoreData NewPlayerScoreData;
	NewPlayerScoreData.TotalNoiseBreakUsage = NoiseBreakUsage;
	PlayerScoreData.Add(TargetPlayer, NewPlayerScoreData);
}
