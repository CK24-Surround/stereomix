// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Data/SMTeam.h"
#include "SMGameState.generated.h"

class USMWidget_RoomId;
DECLARE_DELEGATE_OneParam(FOnChangeTimeSignature, int32 /*RemainTime*/);
DECLARE_DELEGATE_TwoParams(FOnChangeTimeWithRatioSignature, int32 /*RemainTime*/, int32 /*Time*/);
DECLARE_DELEGATE_OneParam(FOnChangePhaseSignature, int32 /*PhaseNumber*/);
DECLARE_DELEGATE_OneParam(FOnChangeTeamScoreSignature, int32 /*TeamScore*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEndRoundSignature, ESMTeam /*VictoryTeam*/);

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

	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// ~Round Time Section
	FORCEINLINE int32 GetReplicatedRemainRoundTime() const { return ReplicatedRemainRoundTime; }

	/** 게임모드에서 게임 스테이트의 남은 라운드 타임을 변경할때 사용합니다. 이외 용도로 사용되면 안 됩니다. */
	FORCEINLINE void SetReplicatedRemainRoundTime(int32 InReplicatedRemainRoundTime) { ReplicatedRemainRoundTime = InReplicatedRemainRoundTime; }

	FOnChangeTimeSignature OnChangeRoundTime;

protected:
	UFUNCTION()
	void OnRep_ReplicatedRemainRoundTime();

	UPROPERTY(ReplicatedUsing = "OnRep_ReplicatedRemainRoundTime")
	int32 ReplicatedRemainRoundTime = 0;
	// ~Round Time Section

	// ~Score Section
public:
	void SetTeamScores(ESMTeam InTeam, int32 InScore);

	FORCEINLINE int32 GetReplicatedEDMTeamScore() const { return ReplicatedEDMTeamScore; }
	FORCEINLINE int32 GetReplicatedFutureBaseTeamScore() const { return ReplicatedFutureBassTeamScore; }

	void SetTeamPhaseScores(ESMTeam InTeam, int32 InScore);

	FORCEINLINE int32 GetReplicatedEDMTeamPhaseScore() const { return ReplicatedEDMTeamPhaseScore; }
	FORCEINLINE int32 GetReplicatedFutureBaseTeamPhaseScore() const { return ReplicatedFutureBassTeamPhaseScore; }

	FOnChangeTeamScoreSignature OnChangeEDMTeamScore;
	FOnChangeTeamScoreSignature OnChangeFutureBassTeamScore;

	FOnChangeTeamScoreSignature OnChangeEDMTeamPhaseScore;
	FOnChangeTeamScoreSignature OnChangeFutureBassTeamPhaseScore;

protected:
	void OnChangeTile(ESMTeam PreviousTeam, ESMTeam NewTeam);

	void AddTeamScore(ESMTeam InTeam);

	void SwapScore(ESMTeam PreviousTeam, ESMTeam NewTeam);

	UFUNCTION()
	void OnRep_ReplicatedEDMTeamScore();

	UFUNCTION()
	void OnRep_ReplicatedFutureBassTeamScore();

	UFUNCTION()
	void OnRep_ReplicatedEDMTeamPhaseScore();

	UFUNCTION()
	void OnRep_ReplicatedFutureBassTeamPhaseScore();

	UPROPERTY(BlueprintReadOnly, Category = "Score")
	TMap<ESMTeam, int32> TeamScores;

	/** TMap은 리플리케이트 되지 않기 때문에 int32를 대신 사용합니다. */
	UPROPERTY(ReplicatedUsing = "OnRep_ReplicatedEDMTeamScore")
	int32 ReplicatedEDMTeamScore = 0;

	/** TMap은 리플리케이트 되지 않기 때문에 int32를 대신 사용합니다. */
	UPROPERTY(ReplicatedUsing = "OnRep_ReplicatedFutureBassTeamScore")
	int32 ReplicatedFutureBassTeamScore = 0;

	int32 TotalTileCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Score")
	TMap<ESMTeam, int32> TeamPhaseScores;

	/** TMap은 리플리케이트 되지 않기 때문에 int32를 대신 사용합니다. */
	UPROPERTY(ReplicatedUsing = "OnRep_ReplicatedEDMTeamPhaseScore")
	int32 ReplicatedEDMTeamPhaseScore = 0;

	/** TMap은 리플리케이트 되지 않기 때문에 int32를 대신 사용합니다. */
	UPROPERTY(ReplicatedUsing = "OnRep_ReplicatedFutureBassTeamPhaseScore")
	int32 ReplicatedFutureBassTeamPhaseScore = 0;
	// ~Score Section

	// ~Phase Section
public:
	FORCEINLINE int32 GetReplicatedRemainPhaseTime() { return ReplicatedRemainPhaseTime; }

	FORCEINLINE void SetReplicatedRemainPhaseTime(int32 InReplicatedRemainPhaseTime) { ReplicatedRemainPhaseTime = InReplicatedRemainPhaseTime; }

	FORCEINLINE int32 GetReplicatedPhaseTime() { return ReplicatedPhaseTime; }

	FORCEINLINE void SetReplicatedPhaseTime(int32 InReplicatedPhaseTime) { ReplicatedPhaseTime = InReplicatedPhaseTime; }

	FORCEINLINE int32 GetReplicatedCurrentPhaseNumber() { return ReplicatedCurrentPhaseNumber; }

	FORCEINLINE void SetReplicatedCurrentPhaseNumber(int32 InReplicatedCurrentPhaseNumber) { ReplicatedCurrentPhaseNumber = InReplicatedCurrentPhaseNumber; }

	void EndPhase();

	FOnChangeTimeWithRatioSignature OnChangePhaseTime;

	FOnChangePhaseSignature OnChangePhase;

protected:
	UFUNCTION()
	void OnRep_ReplicatedRemainPhaseTime();

	UFUNCTION()
	void OnRep_ReplicatedCurrentPhaseNumber();

	UPROPERTY(ReplicatedUsing = "OnRep_ReplicatedRemainPhaseTime")
	int32 ReplicatedRemainPhaseTime = 0;

	UPROPERTY(Replicated)
	int32 ReplicatedPhaseTime = 0;

	UPROPERTY(ReplicatedUsing = "OnRep_ReplicatedCurrentPhaseNumber")
	int32 ReplicatedCurrentPhaseNumber = 0;
	// ~Phase Section

	// ~VictoryDefeat Section
public:
	/** 현재 스코어로 승패 결과를 계산하여 이긴 팀을 반환합니다. */
	ESMTeam CalculateVictoryTeam();

	/** 승패 결과를 출력합니다. 게임모드에서 매치가 종료될때 호출됩니다.*/
	void EndRoundVictoryDefeatResult();

protected:
	/** 클라이언트에서 승패 결과를 확인할 수 있도록 MulticastRPC로 모든 클라이언트에게 전송합니다.*/
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCSendEndRoundResult(ESMTeam VictoryTeam);

public:
	FOnEndRoundSignature OnEndRound;
	// ~VictoryDefeat Section

public:
	UPROPERTY(EditAnywhere, Category="Room")
	TSubclassOf<UUserWidget> RoomIdWidgetClass;

	UPROPERTY()
	TObjectPtr<USMWidget_RoomId> RoomIdWidget;
	
	UPROPERTY(EditAnywhere, ReplicatedUsing=OnRep_ReplicatedShortRoomId, Category="Room")
	FString ShortRoomId;

	UFUNCTION()
	void OnRep_ReplicatedShortRoomId();

	virtual void BeginPlay() override;
};
