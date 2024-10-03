// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Data/SMTeam.h"
#include "FMODBlueprintStatics.h"

#include "SMGameState.generated.h"

class USMScoreMusicManagerComponent;
class USMRoundTimerManagerComponent;
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

	virtual void HandleMatchIsWaitingToStart() override;

	USMTileManagerComponent* GetTileManager() const { return TileManager; }

	USMProjectilePoolManagerComponent* GetProjectilePoolManager() const { return ProjectilePoolManager; }

protected:
	UFUNCTION()
	void OnPreRoundTimeExpiredCallback();

	UFUNCTION()
	void OnRoundTimeExpiredCallback();

	UFUNCTION()
	void OnPostRoundTimeExpiredCallback();

	UPROPERTY(VisibleAnywhere, Category = "Design")
	TObjectPtr<USMRoundTimerManagerComponent> RoundTimerManager;

	UPROPERTY(VisibleAnywhere, Category = "Design")
	TObjectPtr<USMTileManagerComponent> TileManager;

	UPROPERTY(VisibleAnywhere, Category = "Design")
	TObjectPtr<USMScoreMusicManagerComponent> ScoreMusicManager;

	UPROPERTY(VisibleAnywhere, Category = "Design")
	TObjectPtr<USMProjectilePoolManagerComponent> ProjectilePoolManager;
};
