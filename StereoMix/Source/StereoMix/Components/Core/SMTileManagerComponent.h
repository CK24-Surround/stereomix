// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/SMTeam.h"
#include "SMTileManagerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTileScoreDelegate, int32, TileScore);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVictoryTeamDelegate, ESMTeam, VictoryTeam);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTileCapturedDelegate, const AActor*, CapturedInstigator, int32, CapturedTileCount);

class USMScoreMusicManagerComponent;
class ASMTile;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMTileManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USMTileManagerComponent();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void InitializeComponent() override;

	virtual void BeginPlay() override;

	/**
	 * 타일을 점령합니다.
	 * @param TilesToBeCaptured 점령될 타일을 담은 배열입니다.
	 * @param Instigator 점령을 시도하는 액터입니다.
	 * @param OverrideTeamOption 기본적으로 Instigator의 팀으로 점령이 수행되지만 다른 팀으로 덮어 씌워 수행해야하는 경우 여기에 값을 넣어주면 됩니다. (예시: 타일 중립화 ESMTeam::None)
	 * @return 점령에 성공한 타일들을 반환합니다.
	 */
	TArray<ASMTile*> CaptureTiles(const TArray<ASMTile*>& TilesToBeCaptured, const AActor* Instigator, const TOptional<ESMTeam>& OverrideTeamOption = TOptional<ESMTeam>());

	void SetTileScores(ESMTeam Team, int32 Score);

	int32 GetEDMTileScore() const { return EDMTileScore; }

	int32 GetFBTileScore() const { return FBTileScore; }

	void ShowGameResult();

	FTileScoreDelegate OnEDMTileScoreChanged;

	FTileScoreDelegate OnFBTileScoreChanged;

	FVictoryTeamDelegate OnVictoryTeamAnnounced;

	FOnTileCapturedDelegate OnTilesCaptured;

protected:
	void OnTileChanged(ESMTeam PreviousTeam, ESMTeam NewTeam);

	/** 오너가 ScoreMusicManager를 가지고 있다면 ScoreMusic을 변경합니다. */
	void ChangeScoreMusic();

	void NeutralizeTile(ESMTeam PreviousTeam);

	void AddTileScore(ESMTeam Team);

	void SwapTileScore(ESMTeam PreviousTeam, ESMTeam NewTeam);

	UFUNCTION()
	void OnRep_EDMTileScore();

	UFUNCTION()
	void OnRep_FBTileScore();

	ESMTeam CalculateVictoryTeam();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSendGameResult(ESMTeam VictoryTeam);

	UPROPERTY(EditAnywhere, Category = "Design")
	uint32 bShowDebug:1 = false;

	UPROPERTY(BlueprintReadOnly, Category = "Score")
	TMap<ESMTeam, int32> TileScores;

	UPROPERTY(ReplicatedUsing = "OnRep_EDMTileScore")
	int32 EDMTileScore;

	UPROPERTY(ReplicatedUsing = "OnRep_FBTileScore")
	int32 FBTileScore;

	TWeakObjectPtr<USMScoreMusicManagerComponent> CachedScoreMusicManager;
};
