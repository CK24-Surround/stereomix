// Copyright Studio Surround. All Rights Reserved.


#include "SMScoreManagerComponent.h"

#include "GameFramework/GameStateBase.h"
#include "SMTileManagerComponent.h"
#include "StereoMixLog.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"


void USMScoreManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	const UWorld* World = GetWorld();
	const AGameStateBase* GameState = World ? World->GetGameState() : nullptr;
	if (USMTileManagerComponent* TileManager = GameState ? GameState->GetComponentByClass<USMTileManagerComponent>() : nullptr)
	{
		TileManager->OnTilesCaptured.AddDynamic(this, &ThisClass::AddTotalCapturedTiles);
	}
}

void USMScoreManagerComponent::AddTotalCapturedTiles(const AActor* CapturedInstigator, int32 CapturedTilesCount)
{
	if (!CapturedInstigator)
	{
		return;
	}

	PlayerScoreData.FindOrAdd(CapturedInstigator).TotalCapturedTiles += CapturedTilesCount;

	UE_LOG(LogStereoMix, Warning, TEXT("[%s] Tiles: %d, DamageDealt: %f, DamageReceived: %f, Death: %d, Kill: %d, NoiseBreak: %d, TotalScore: %d"),
		*GetNameSafe(CapturedInstigator),
		PlayerScoreData[CapturedInstigator].TotalCapturedTiles,
		PlayerScoreData[CapturedInstigator].TotalDamageDealt,
		PlayerScoreData[CapturedInstigator].TotalDamageReceived,
		PlayerScoreData[CapturedInstigator].TotalDeathCount,
		PlayerScoreData[CapturedInstigator].TotalKillCount,
		PlayerScoreData[CapturedInstigator].TotalNoiseBreakUsage,
		PlayerScoreData[CapturedInstigator].TotalScore());
}

void USMScoreManagerComponent::AddTotalDamageDealt(const AActor* TargetPlayer, float DamageDealt)
{
	if (!TargetPlayer)
	{
		return;
	}

	PlayerScoreData.FindOrAdd(TargetPlayer).TotalDamageDealt += DamageDealt;

	UE_LOG(LogStereoMix, Warning, TEXT("[%s] Tiles: %d, DamageDealt: %f, DamageReceived: %f, Death: %d, Kill: %d, NoiseBreak: %d, TotalScore: %d"),
		*GetNameSafe(TargetPlayer),
		PlayerScoreData[TargetPlayer].TotalCapturedTiles,
		PlayerScoreData[TargetPlayer].TotalDamageDealt,
		PlayerScoreData[TargetPlayer].TotalDamageReceived,
		PlayerScoreData[TargetPlayer].TotalDeathCount,
		PlayerScoreData[TargetPlayer].TotalKillCount,
		PlayerScoreData[TargetPlayer].TotalNoiseBreakUsage,
		PlayerScoreData[TargetPlayer].TotalScore());
}

void USMScoreManagerComponent::AddTotalDamageReceived(const AActor* TargetPlayer, float DamageReceived)
{
	if (!TargetPlayer)
	{
		return;
	}

	PlayerScoreData.FindOrAdd(TargetPlayer).TotalDamageReceived += DamageReceived;

	UE_LOG(LogStereoMix, Warning, TEXT("[%s] Tiles: %d, DamageDealt: %f, DamageReceived: %f, Death: %d, Kill: %d, NoiseBreak: %d, TotalScore: %d"),
		*GetNameSafe(TargetPlayer),
		PlayerScoreData[TargetPlayer].TotalCapturedTiles,
		PlayerScoreData[TargetPlayer].TotalDamageDealt,
		PlayerScoreData[TargetPlayer].TotalDamageReceived,
		PlayerScoreData[TargetPlayer].TotalDeathCount,
		PlayerScoreData[TargetPlayer].TotalKillCount,
		PlayerScoreData[TargetPlayer].TotalNoiseBreakUsage,
		PlayerScoreData[TargetPlayer].TotalScore());
}

void USMScoreManagerComponent::AddTotalDeathCount(const AActor* TargetPlayer)
{
	if (!TargetPlayer)
	{
		return;
	}

	PlayerScoreData.FindOrAdd(TargetPlayer).TotalDeathCount += 1;

	UE_LOG(LogStereoMix, Warning, TEXT("[%s] Tiles: %d, DamageDealt: %f, DamageReceived: %f, Death: %d, Kill: %d, NoiseBreak: %d, TotalScore: %d"),
		*GetNameSafe(TargetPlayer),
		PlayerScoreData[TargetPlayer].TotalCapturedTiles,
		PlayerScoreData[TargetPlayer].TotalDamageDealt,
		PlayerScoreData[TargetPlayer].TotalDamageReceived,
		PlayerScoreData[TargetPlayer].TotalDeathCount,
		PlayerScoreData[TargetPlayer].TotalKillCount,
		PlayerScoreData[TargetPlayer].TotalNoiseBreakUsage,
		PlayerScoreData[TargetPlayer].TotalScore());
}

void USMScoreManagerComponent::AddTotalKillCount(const AActor* TargetPlayer)
{
	if (!TargetPlayer)
	{
		return;
	}

	PlayerScoreData.FindOrAdd(TargetPlayer).TotalKillCount += 1;

	UE_LOG(LogStereoMix, Warning, TEXT("[%s] Tiles: %d, DamageDealt: %f, DamageReceived: %f, Death: %d, Kill: %d, NoiseBreak: %d, TotalScore: %d"),
		*GetNameSafe(TargetPlayer),
		PlayerScoreData[TargetPlayer].TotalCapturedTiles,
		PlayerScoreData[TargetPlayer].TotalDamageDealt,
		PlayerScoreData[TargetPlayer].TotalDamageReceived,
		PlayerScoreData[TargetPlayer].TotalDeathCount,
		PlayerScoreData[TargetPlayer].TotalKillCount,
		PlayerScoreData[TargetPlayer].TotalNoiseBreakUsage,
		PlayerScoreData[TargetPlayer].TotalScore());
}

void USMScoreManagerComponent::AddTotalNoiseBreakUsage(const AActor* TargetPlayer)
{
	if (!TargetPlayer)
	{
		return;
	}

	PlayerScoreData.FindOrAdd(TargetPlayer).TotalNoiseBreakUsage += 1;

	UE_LOG(LogStereoMix, Warning, TEXT("[%s] Tiles: %d, DamageDealt: %f, DamageReceived: %f, Death: %d, Kill: %d, NoiseBreak: %d, TotalScore: %d"),
		*GetNameSafe(TargetPlayer),
		PlayerScoreData[TargetPlayer].TotalCapturedTiles,
		PlayerScoreData[TargetPlayer].TotalDamageDealt,
		PlayerScoreData[TargetPlayer].TotalDamageReceived,
		PlayerScoreData[TargetPlayer].TotalDeathCount,
		PlayerScoreData[TargetPlayer].TotalKillCount,
		PlayerScoreData[TargetPlayer].TotalNoiseBreakUsage,
		PlayerScoreData[TargetPlayer].TotalScore());
}

const AActor* USMScoreManagerComponent::GetMVPPlayer(ESMTeam Team) const
{
	const AActor* MVPPlayer = nullptr;
	int32 MaxScore = 0;

	for (const auto& [PlayerActor, ScoreData] : PlayerScoreData)
	{
		const ASMPlayerCharacterBase* Player = Cast<ASMPlayerCharacterBase>(PlayerActor);
		if (!Player || Player->GetTeam() != Team)
		{
			continue;
		}

		if (ScoreData.TotalScore() > MaxScore)
		{
			MaxScore = ScoreData.TotalScore();
			MVPPlayer = PlayerActor;
		}
	}

	return MVPPlayer;
}
