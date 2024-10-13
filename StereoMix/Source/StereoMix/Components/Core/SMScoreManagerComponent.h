// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/SMCharacterType.h"
#include "Data/SMTeam.h"
#include "SMScoreManagerComponent.generated.h"


class ASMPlayerCharacterBase;

USTRUCT(BlueprintType)
struct FPlayerScoreData
{
	GENERATED_BODY()

	ESMTeam PlayerTeam = ESMTeam::None;

	ESMCharacterType CharacterType = ESMCharacterType::None;

	FString PlayerName;

	int32 TotalCapturedTiles = 0;

	float TotalDamageDealt = 0.0f;

	float TotalDamageReceived = 0.0f;

	int32 TotalDeathCount = 0;

	int32 TotalKillCount = 0;

	int32 TotalNoiseBreakUsage = 0;

	float CorrectionValue = 0.4f;

	int32 TotalScore() const
	{
		const int32 KillScore = TotalKillCount == 0 ? 1 : TotalKillCount * CorrectionValue;
		const float DamageDealtScore = TotalDamageDealt == 0.0f ? KillScore : TotalDamageDealt + (KillScore * 1.5f);
		const int32 NoiseBreakUsageScore = TotalNoiseBreakUsage == 0 ? 1 : (TotalNoiseBreakUsage + (TotalNoiseBreakUsage * CorrectionValue)) * 10;
		const int32 CapturedTilesScore = TotalCapturedTiles + (TotalCapturedTiles * CorrectionValue);
		return CapturedTilesScore + DamageDealtScore + NoiseBreakUsageScore;
	}
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMScoreManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UFUNCTION()
	void AddTotalCapturedTiles(const AActor* CapturedInstigator, int32 CapturedTilesCount);

	UFUNCTION()
	void AddTotalDamageDealt(const AActor* TargetPlayer, float DamageDealt);

	UFUNCTION()
	void AddTotalDamageReceived(const AActor* TargetPlayer, float DamageReceived);

	UFUNCTION()
	void AddTotalDeathCount(const AActor* TargetPlayer);

	UFUNCTION()
	void AddTotalKillCount(const AActor* TargetPlayer);

	UFUNCTION()
	void AddTotalNoiseBreakUsage(const AActor* TargetPlayer);

	TWeakObjectPtr<const AActor> GetMVPPlayer(ESMTeam Team) const;

	UFUNCTION()
	void LogAllPlayerData();

protected:
	static FPlayerScoreData GetDefaultPlayerScoreData(const TWeakObjectPtr<const AActor>& TargetPlayer);

	void LogPlayerData(const TWeakObjectPtr<const AActor>& TargetPlayer) const;
	
	TMap<TWeakObjectPtr<const AActor>, FPlayerScoreData> PlayerScoreData;
};
