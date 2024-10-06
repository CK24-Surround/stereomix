// Copyright Studio Surround. All Rights Reserved.


#include "SMScoreManagerComponent.h"

#include "GameFramework/GameStateBase.h"
#include "SMTileManagerComponent.h"
#include "StereoMixLog.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"


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
		UE_LOG(LogStereoMix, Warning, TEXT("[%s] Tiles: %d, DamageDealt: %f, DamageReceived: %f, Death: %d, Kill: %d, NoiseBreak: %d, TotalScore: %d"),
			*GetNameSafe(CapturedInstigator),
			PlayerScoreData[CapturedInstigator].TotalCapturedTiles,
			PlayerScoreData[CapturedInstigator].TotalDamageDealt,
			PlayerScoreData[CapturedInstigator].TotalDamageReceived,
			PlayerScoreData[CapturedInstigator].TotalDeathCount,
			PlayerScoreData[CapturedInstigator].TotalKillCount,
			PlayerScoreData[CapturedInstigator].TotalNoiseBreakUsage,
			PlayerScoreData[CapturedInstigator].TotalScore());
		return;
	}

	FPlayerScoreData NewPlayerScoreData;
	NewPlayerScoreData.TotalCapturedTiles = CapturedTilesCount;
	PlayerScoreData.Add(CapturedInstigator, NewPlayerScoreData);
}

void USMScoreManagerComponent::AddTotalDamageDealt(const AActor* TargetPlayer, float DamageDealt)
{
	if (!TargetPlayer)
	{
		return;
	}

	if (PlayerScoreData.Contains(TargetPlayer))
	{
		PlayerScoreData[TargetPlayer].TotalDamageDealt += DamageDealt;
		UE_LOG(LogStereoMix, Warning, TEXT("[%s] Tiles: %d, DamageDealt: %f, DamageReceived: %f, Death: %d, Kill: %d, NoiseBreak: %d, TotalScore: %d"),
			*GetNameSafe(TargetPlayer),
			PlayerScoreData[TargetPlayer].TotalCapturedTiles,
			PlayerScoreData[TargetPlayer].TotalDamageDealt,
			PlayerScoreData[TargetPlayer].TotalDamageReceived,
			PlayerScoreData[TargetPlayer].TotalDeathCount,
			PlayerScoreData[TargetPlayer].TotalKillCount,
			PlayerScoreData[TargetPlayer].TotalNoiseBreakUsage,
			PlayerScoreData[TargetPlayer].TotalScore());
		return;
	}

	FPlayerScoreData NewPlayerScoreData;
	NewPlayerScoreData.TotalDamageDealt = DamageDealt;
	PlayerScoreData.Add(TargetPlayer, NewPlayerScoreData);
}

void USMScoreManagerComponent::AddTotalDamageReceived(const AActor* TargetPlayer, float DamageReceived)
{
	if (!TargetPlayer)
	{
		return;
	}

	if (PlayerScoreData.Contains(TargetPlayer))
	{
		PlayerScoreData[TargetPlayer].TotalDamageReceived += DamageReceived;
		UE_LOG(LogStereoMix, Warning, TEXT("[%s] Tiles: %d, DamageDealt: %f, DamageReceived: %f, Death: %d, Kill: %d, NoiseBreak: %d, TotalScore: %d"),
			*GetNameSafe(TargetPlayer),
			PlayerScoreData[TargetPlayer].TotalCapturedTiles,
			PlayerScoreData[TargetPlayer].TotalDamageDealt,
			PlayerScoreData[TargetPlayer].TotalDamageReceived,
			PlayerScoreData[TargetPlayer].TotalDeathCount,
			PlayerScoreData[TargetPlayer].TotalKillCount,
			PlayerScoreData[TargetPlayer].TotalNoiseBreakUsage,
			PlayerScoreData[TargetPlayer].TotalScore());
		return;
	}

	FPlayerScoreData NewPlayerScoreData;
	NewPlayerScoreData.TotalDamageReceived = DamageReceived;
	PlayerScoreData.Add(TargetPlayer, NewPlayerScoreData);
}

void USMScoreManagerComponent::AddTotalDeathCount(const AActor* TargetPlayer, int32 DeathCount)
{
	if (!TargetPlayer)
	{
		return;
	}

	if (PlayerScoreData.Contains(TargetPlayer))
	{
		PlayerScoreData[TargetPlayer].TotalDeathCount += DeathCount;
		UE_LOG(LogStereoMix, Warning, TEXT("[%s] Tiles: %d, DamageDealt: %f, DamageReceived: %f, Death: %d, Kill: %d, NoiseBreak: %d, TotalScore: %d"),
			*GetNameSafe(TargetPlayer),
			PlayerScoreData[TargetPlayer].TotalCapturedTiles,
			PlayerScoreData[TargetPlayer].TotalDamageDealt,
			PlayerScoreData[TargetPlayer].TotalDamageReceived,
			PlayerScoreData[TargetPlayer].TotalDeathCount,
			PlayerScoreData[TargetPlayer].TotalKillCount,
			PlayerScoreData[TargetPlayer].TotalNoiseBreakUsage,
			PlayerScoreData[TargetPlayer].TotalScore());
		return;
	}

	FPlayerScoreData NewPlayerScoreData;
	NewPlayerScoreData.TotalDeathCount = DeathCount;
	PlayerScoreData.Add(TargetPlayer, NewPlayerScoreData);
}

void USMScoreManagerComponent::AddTotalKillCount(const AActor* TargetPlayer, int32 KillCount)
{
	if (!TargetPlayer)
	{
		return;
	}

	if (PlayerScoreData.Contains(TargetPlayer))
	{
		PlayerScoreData[TargetPlayer].TotalKillCount += KillCount;
		UE_LOG(LogStereoMix, Warning, TEXT("[%s] Tiles: %d, DamageDealt: %f, DamageReceived: %f, Death: %d, Kill: %d, NoiseBreak: %d, TotalScore: %d"),
			*GetNameSafe(TargetPlayer),
			PlayerScoreData[TargetPlayer].TotalCapturedTiles,
			PlayerScoreData[TargetPlayer].TotalDamageDealt,
			PlayerScoreData[TargetPlayer].TotalDamageReceived,
			PlayerScoreData[TargetPlayer].TotalDeathCount,
			PlayerScoreData[TargetPlayer].TotalKillCount,
			PlayerScoreData[TargetPlayer].TotalNoiseBreakUsage,
			PlayerScoreData[TargetPlayer].TotalScore());
		return;
	}

	FPlayerScoreData NewPlayerScoreData;
	NewPlayerScoreData.TotalKillCount = KillCount;
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
		UE_LOG(LogStereoMix, Warning, TEXT("[%s] Tiles: %d, DamageDealt: %f, DamageReceived: %f, Death: %d, Kill: %d, NoiseBreak: %d, TotalScore: %d"),
			*GetNameSafe(TargetPlayer),
			PlayerScoreData[TargetPlayer].TotalCapturedTiles,
			PlayerScoreData[TargetPlayer].TotalDamageDealt,
			PlayerScoreData[TargetPlayer].TotalDamageReceived,
			PlayerScoreData[TargetPlayer].TotalDeathCount,
			PlayerScoreData[TargetPlayer].TotalKillCount,
			PlayerScoreData[TargetPlayer].TotalNoiseBreakUsage,
			PlayerScoreData[TargetPlayer].TotalScore());
		return;
	}

	FPlayerScoreData NewPlayerScoreData;
	NewPlayerScoreData.TotalNoiseBreakUsage = NoiseBreakUsage;
	PlayerScoreData.Add(TargetPlayer, NewPlayerScoreData);
}

const AActor* USMScoreManagerComponent::GetMVPPlayer(ESMTeam team) const
{
	const AActor* MVPPlayer = nullptr;
	int32 MaxScore = 0;

	for (const auto& PlayerScore : PlayerScoreData)
	{
		const ASMPlayerCharacterBase* Player = Cast<ASMPlayerCharacterBase>(PlayerScore.Key);
		if (!Player || Player->GetTeam() != team)
		{
			continue;
		}

		if (PlayerScore.Value.TotalScore() > MaxScore)
		{
			MaxScore = PlayerScore.Value.TotalScore();
			MVPPlayer = PlayerScore.Key;
		}
	}

	return MVPPlayer;
}
