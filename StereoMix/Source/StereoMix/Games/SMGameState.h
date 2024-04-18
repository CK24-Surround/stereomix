// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Utilities/SMTeam.h"
#include "SMGameState.generated.h"

DECLARE_DELEGATE_OneParam(FOnChangeRoundTime, int32 /*RoundTime*/);

class USMDesignData;
/**
 * 
 */
UCLASS()
class STEREOMIX_API ASMGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ASMGameState();

public:
	virtual void PostInitializeComponents() override;

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY()
	TObjectPtr<USMDesignData> DesignData;

// ~Score Section
public:
	FORCEINLINE int32 GetFutureBaseTeamScore() { return TeamScores[ESMTeam::FutureBass]; }
	FORCEINLINE int32 GetEDMTeamScore() { return TeamScores[ESMTeam::EDM]; }

protected:
	void OnChangeTile(ESMTeam PreviousTeam, ESMTeam NewTeam);

	void AddTeamScore(ESMTeam InTeam);

	void SwapScore(ESMTeam PreviousTeam, ESMTeam NewTeam);

	// 점수를 출력하는 임시 코드입니다.
	void PrintScore();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Score")
	TMap<ESMTeam, int32> TeamScores;

	int32 TotalTileCount = 0;
// ~Score Section

// ~Round Time Section
public:
	FOnChangeRoundTime OnChangeRoundTime;

protected:
	void PerformRoundTime();

protected:
	FTimerHandle RoundTimerHandle;
	int32 RoundTime = 0.0f;
	int32 RemainRoundTime = 0.0f;
// ~Round Time Section
};
