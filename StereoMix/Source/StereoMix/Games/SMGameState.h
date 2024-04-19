// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Utilities/SMTeam.h"
#include "SMGameState.generated.h"

DECLARE_DELEGATE_OneParam(FOnChangeRoundTimeSignature, int32 /*RoundTime*/);
DECLARE_DELEGATE_OneParam(FOnChangeTeamScoreSignature, int32 /*TeamScore*/);
DECLARE_DELEGATE(FOnEndTimerSignature);
DECLARE_DELEGATE(FOnEndRoundSignature);

class USMDesignData;
/**
 * 
 */
UCLASS()
class STEREOMIX_API ASMGameState : public AGameState
{
	GENERATED_BODY()

public:
	ASMGameState();

public:
	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY()
	TObjectPtr<USMDesignData> DesignData;

// ~Score Section
public:
	void SetTeamScores(ESMTeam InTeam, int32 InScore);

	FORCEINLINE int32 GetReplicatedEDMTeamScore() const { return ReplicatedEDMTeamScore; }
	FORCEINLINE int32 GetReplicatedFutureBaseTeamScore() const { return ReplicatedFutureBaseTeamScore; }

	FOnChangeTeamScoreSignature OnChangeEDMTeamScore;
	FOnChangeTeamScoreSignature OnChangeFutureBaseTeamScore;

protected:
	void OnChangeTile(ESMTeam PreviousTeam, ESMTeam NewTeam);

	void AddTeamScore(ESMTeam InTeam);

	void SwapScore(ESMTeam PreviousTeam, ESMTeam NewTeam);

	UFUNCTION()
	void OnRep_ReplicatedEDMTeamScore();

	UFUNCTION()
	void OnRep_ReplicatedFutureBaseTeamScore();

	// 점수를 출력하는 임시 코드입니다.
	void PrintScore();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Score")
	TMap<ESMTeam, int32> TeamScores;

	/** TMap은 리플리케이트 되지 않기 때문에 사용됩니다. */
	UPROPERTY(ReplicatedUsing = "OnRep_ReplicatedEDMTeamScore")
	int32 ReplicatedEDMTeamScore;

	/** TMap은 리플리케이트 되지 않기 때문에 사용됩니다. */
	UPROPERTY(ReplicatedUsing = "OnRep_ReplicatedFutureBaseTeamScore")
	int32 ReplicatedFutureBaseTeamScore;

	int32 TotalTileCount = 0;
// ~Score Section

// ~Round Time Section
public:
	FORCEINLINE int32 GetRemainRoundTime() const { return RemainRoundTime; }

	void SetRemainRoundTime(int32 InRemainRoundTime);

public:
	FOnEndTimerSignature OnEndRoundTimer;
	// TODO: 나중에 레벨 내 승패 결과 장소로 이동하는 기능이 생기면 없앨 예정입니다.
	FOnEndTimerSignature OnEndVictoryDefeatTimer;

public:
	FOnChangeRoundTimeSignature OnChangeRoundTime;

protected:
	void PerformRoundTime();

	UFUNCTION()
	void OnRep_RemainRoundTime();

protected:
	FTimerHandle RoundTimerHandle;
	int32 RoundTime = 0;

	UPROPERTY(ReplicatedUsing = "OnRep_RemainRoundTime")
	int32 RemainRoundTime = 0;

	int32 VictoryDefeatTime = 0;

	uint32 bIsRoundEnd = false;
// ~Round Time Section

// ~VictoryDefeat Section
public:
	void EndRound();

protected:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCEndRound();

public:
	FOnEndRoundSignature OnEndRound;
// ~VictoryDefeat Section
};
