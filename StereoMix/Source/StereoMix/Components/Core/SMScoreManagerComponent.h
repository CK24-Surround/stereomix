// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/SMCharacterType.h"
#include "Data/SMTeam.h"
#include "SMScoreManagerComponent.generated.h"


class USMPlaylist;
class ASMPlayerCharacterBase;

USTRUCT(BlueprintType)
struct FPlayerScoreData
{
	GENERATED_BODY()

	UPROPERTY()
	ESMTeam PlayerTeam = ESMTeam::None;

	UPROPERTY()
	ESMCharacterType CharacterType = ESMCharacterType::None;

	UPROPERTY()
	FString PlayerName;

	UPROPERTY()
	int32 TotalCapturedTiles = 0;

	UPROPERTY()
	float TotalDamageDealt = 0.0f;
	
	UPROPERTY()
	float TotalDamageReceived = 0.0f;

	UPROPERTY()
	int32 TotalKillCount = 0;

	UPROPERTY()
	int32 TotalDeathCount = 0;

	UPROPERTY()
	int32 TotalNoiseBreakUsage = 0;

	UPROPERTY()
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
	USMScoreManagerComponent();

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
	
	TArray<FPlayerScoreData> GetTeamScoreData(ESMTeam Team);

	UFUNCTION()
	void LogAllPlayerData();

	UFUNCTION()
	void OnVictoryTeamAnnouncedCallback(ESMTeam VictoryTeam);
	
protected:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastShowPlaylist(ESMTeam WinTeam, const TArray<FPlayerScoreData>& EDMPlayerData, const TArray<FPlayerScoreData>& FBPlayerData);
	
	UPROPERTY(EditAnywhere, Category = "Design|UI|HUD")
	TSubclassOf<USMPlaylist> PlaylistWidgetClass;

	UPROPERTY()
	TObjectPtr<USMPlaylist> PlaylistWidget;
	
	static FPlayerScoreData GetDefaultPlayerScoreData(const TWeakObjectPtr<const AActor>& TargetPlayer);

	void LogPlayerData(const TWeakObjectPtr<const AActor>& TargetPlayer) const;

	TMap<TWeakObjectPtr<const AActor>, FPlayerScoreData> PlayerScoreData;
};
