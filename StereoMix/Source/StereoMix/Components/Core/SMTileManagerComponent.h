// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/SMTeam.h"
#include "SMTileManagerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTileScoreDelegate, int32, TileScore);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVictoryTeamDelegate, ESMTeam, VictoryTeam);

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

	/**
	 * 타일을 점령합니다.
	 * @param StartTile 시작 타일입니다.
	 * @param InstigatorTeam 점령을 시도하는 오브젝트의 팀입니다.
	 * @param HalfHorizenSize 가로측 점령 사이즈입니다.
	 * @param HalfVerticalSize 세로측 점령 사이즈입니다.
	 * @return 점령에 성공한 타일의 개수를 반환합니다.
	 */
	int32 TileCapture(ASMTile* StartTile, ESMTeam InstigatorTeam, float HalfHorizenSize, float HalfVerticalSize, bool bShowDebug = false);

	void SetTileScores(ESMTeam Team, int32 Score);

	int32 GetEDMTileScore() const { return EDMTileScore; }

	int32 GetFBTileScore() const { return FBTileScore; }

	void ShowGameResult();

	FTileScoreDelegate OnEDMTileScoreChanged;

	FTileScoreDelegate OnFBTileScoreChanged;

	FVictoryTeamDelegate OnVictoryTeamAnnounced;

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

	UPROPERTY(BlueprintReadOnly, Category = "Score")
	TMap<ESMTeam, int32> TileScores;

	UPROPERTY(ReplicatedUsing = "OnRep_EDMTileScore")
	int32 EDMTileScore;

	UPROPERTY(ReplicatedUsing = "OnRep_FBTileScore")
	int32 FBTileScore;

	TWeakObjectPtr<USMScoreMusicManagerComponent> CachedScoreMusicManager;
};
