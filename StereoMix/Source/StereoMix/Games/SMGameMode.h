// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "SMGameMode.generated.h"

class USMProjectilPool;
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

protected:
	/** 플레이어 입장시 전송된 닉네임으로 플레이어 스테이트를 초기화해줍니다. */
	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;

protected:
	virtual void PostInitializeComponents() override;

protected:
	virtual void StartMatch() override;

	virtual void EndMatch() override;

protected:
	void BindToGameState();

	/** 승패 확인 타이머가 만료된 후 처리를 합니다. */
	void EndVictoryDefeatTimer();

protected:
	UPROPERTY()
	TWeakObjectPtr<ASMGameState> CachedSMGameState;

// ~Round Time Section
protected:
	/** 남은 라운스 타임을 설정합니다. GameState에 복제됩니다. */
	void SetRemainRoundTime(int32 InRemainRoundTime);

	/** 1초 주기로 라운드 타임과 관련된 처리를 합니다. */
	void PerformRoundTime();

protected:
	FTimerHandle RoundTimerHandle;

	int32 RemainRoundTime = 0;

	UPROPERTY(EditAnywhere, Category = "Rule|Round", DisplayName = "라운드 당 시간(초)")
	int32 RoundTime = 240;

	UPROPERTY(EditAnywhere, Category = "Rule|Result", DisplayName = "결과 확인 시간(초)")
	int32 VictoryDefeatTime = 15;

	uint32 bIsRoundEnd = false;
// ~Round Time Section

// ~Phase Section
protected:
	FORCEINLINE int32 GetPhaseTime() { return PhaseTime; }

	/** 페이즈의 남은 시간을 설정합니다. */
	void SetRemainPhaseTime(int32 InRemainPhaseTime);

	/** 1초 주기로 페이즈 타임과 관련된 처리를 합니다. */
	void PerformPhaseTime();

	/** 현재 페이즈 수를 설정합니다. */
	void SetCurrentPhaseNumber(int32 InCurrentPhaseNumber);

protected:
	FTimerHandle PhaseTimerHandle;

	int32 RemainPhaseTime = 0;

	UPROPERTY(EditAnywhere, Category = "Rule|Phase", DisplayName = "페이즈 당 시간(초)")
	int32 PhaseTime = 60;

	int32 CurrentPhaseNumber = 0;
// ~Phase Section

// ~Object Polling Section
protected:
	UPROPERTY()
	TObjectPtr<USMProjectilPool> ProjectilePool;
// ~Object Polling Section
};
