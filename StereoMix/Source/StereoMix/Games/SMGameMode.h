// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Data/SMTeam.h"
#include "SMGameMode.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnStartMatchSignature);

class ASMProjectile;
class USMProjectilePool;
class ASMGameState;
/**
 * 
 */
UCLASS()
class STEREOMIX_API ASMGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ASMGameMode();

	virtual void BeginPlay() override;

protected:
	/** 플레이어 입장시 전송된 닉네임으로 플레이어 스테이트를 초기화해줍니다. */
	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;

	virtual void PostInitializeComponents() override;

	virtual void StartMatch() override;

	virtual void EndMatch() override;

public:
	FOnStartMatchSignature OnStartMatch;

protected:
	void BindToGameState();

	/** 승패 확인 타이머가 만료된 후 처리를 합니다. */
	void EndVictoryDefeatTimer();

protected:
	TWeakObjectPtr<ASMGameState> CachedSMGameState;

	/** 시간이 만료되면 현재 레벨을 재시작할지 여부입니다. 테스트용으로 사용됩니다. */
	UPROPERTY(EditAnywhere, Category = "Design|Test")
	uint32 bUseRestart:1 = false;

// ~Wait And Countdown Section
protected:
	/** 대기 시간 만료시 호출됩니다. */
	void OnWaitTimeEndCallback();

	void SetRemainCountdownTime(int32 InRemainCountdownTime);

	/** 매 초 카운트 다운 시간을 감소시킵니다. */
	void PerformCountdownTime();

	/** 카운트 다운 만료시 호출됩니다. */
	void OnCountdownTimeEnd();

protected:
	/** 게임 시작 후 대기 시간입니다. */
	UPROPERTY(EditAnywhere, Category = "Design|Wait")
	int32 WaitTime = 10;

	FTimerHandle CountdownTimeHandle;

	int32 CountdownTime = 3;

	int32 RemainCountdownTime = 0;

// ~Wait And Countdown Section

// ~Round Time Section
protected:
	/** 라운드 시작를 시작합니다. */
	void StartRound();

	/** 남은 라운스 타임을 설정합니다. GameState에 복제됩니다. */
	void SetRemainRoundTime(int32 InRemainRoundTime);

	/** 1초 주기로 라운드 타임과 관련된 처리를 합니다. */
	void PerformRoundTime();

protected:
	FTimerHandle RoundTimerHandle;

	int32 RemainRoundTime = 0;

	UPROPERTY(EditAnywhere, Category = "Design|Rule|Round", DisplayName = "라운드 당 시간(초)")
	int32 RoundTime = 240;

	UPROPERTY(EditAnywhere, Category = "Design|Rule|Result", DisplayName = "결과 확인 시간(초)")
	int32 VictoryDefeatTime = 15;

	uint32 bIsRoundEnd = false;
// ~Round Time Section

// ~Phase Section
public:
	FORCEINLINE int32 GetPhaseTime() { return PhaseTime; }

protected:
	/** 페이즈의 남은 시간을 설정합니다. */
	void SetRemainPhaseTime(int32 InRemainPhaseTime);

	/** 1초 주기로 페이즈 타임과 관련된 처리를 합니다. */
	void PerformPhaseTime();

	/** 현재 페이즈 수를 설정합니다. */
	void SetCurrentPhaseNumber(int32 InCurrentPhaseNumber);

protected:
	FTimerHandle PhaseTimerHandle;

	int32 RemainPhaseTime = 0;

	UPROPERTY(EditAnywhere, Category = "Design|Rule|Phase", DisplayName = "페이즈 당 시간(초)")
	int32 PhaseTime = 60;

	int32 CurrentPhaseNumber = 0;
// ~Phase Section

// ~Object Pooling Section
public:
	USMProjectilePool* GetEletricGuitarProjectilePool(ESMTeam SourceTeam);

protected:
	void InitProjectilePool();

	UPROPERTY(EditAnywhere, Category = "Design|Pool|Class")
	TMap<ESMTeam, TSubclassOf<USMProjectilePool>> EletricGuitarProjectilePoolClass;

	UPROPERTY()
	TMap<ESMTeam, TObjectPtr<USMProjectilePool>> EletricGuitarProjectilePool;
// ~Object Pooling Section
};
