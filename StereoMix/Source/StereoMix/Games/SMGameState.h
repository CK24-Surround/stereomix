// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Data/SMTeam.h"
#include "FMODBlueprintStatics.h"

#include "SMGameState.generated.h"

class USMRoundTimerComponent;
class USMProjectilePoolManagerComponent;
class USMTileManagerComponent;
class UFMODEvent;
class USMWidget_RoomId;

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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PostInitializeComponents() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void HandleMatchIsWaitingToStart() override;

	virtual void HandleMatchHasEnded() override;

	USMTileManagerComponent* GetTileManager() { return TileManager; }

	USMProjectilePoolManagerComponent* GetProjectilePoolManager() { return ProjectilePoolManager; }

protected:
	UFUNCTION()
	void OnPreRoundTimeExpiredCallback();

	UFUNCTION()
	void OnRoundTimeExpiredCallback();

	UFUNCTION()
	void OnPostRoundTimeExpiredCallback();

	UPROPERTY(VisibleAnywhere, Category = "Design")
	TObjectPtr<USMRoundTimerComponent> RoundTimer;

	UPROPERTY(VisibleAnywhere, Category = "Design")
	TObjectPtr<USMTileManagerComponent> TileManager;

	UPROPERTY(VisibleAnywhere, Category = "Design")
	TObjectPtr<USMProjectilePoolManagerComponent> ProjectilePoolManager;

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

	// ~Sound Section
	void PlayBackgroundMusic();

	ESMTeam GetCurrentMusicOwner() const { return ReplicatedCurrentMusicOwner; }
	void SetMusicOwner(ESMTeam InTeam);

private:
	const float BGM_PARAMETER_NONE = 0.f;
	const float BGM_PARAMETER_FUTURE_BASS = 1.f;
	const float BGM_PARAMETER_EDM = 2.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound", meta = (AllowPrivateAccess))
	TObjectPtr<UFMODEvent> BackgroundMusic;

	UPROPERTY(BlueprintReadOnly, Category = "Sound", meta = (AllowPrivateAccess))
	FFMODEventInstance BackgroundMusicEventInstance;

	UPROPERTY(ReplicatedUsing=OnRep_ReplicatedCurrentMusicPlayer)
	ESMTeam ReplicatedCurrentMusicOwner = ESMTeam::None;

	UFUNCTION()
	void OnRep_ReplicatedCurrentMusicPlayer();
	// ~Sound Section
};
