// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Utilities/SMTeam.h"
#include "SMGameState.generated.h"

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
};
