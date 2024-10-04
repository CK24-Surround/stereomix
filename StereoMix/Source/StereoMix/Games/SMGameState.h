// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Data/Enum/SMRoundState.h"

#include "SMGameState.generated.h"

class USMScoreMusicManagerComponent;
class USMRoundTimerManagerComponent;
class USMProjectilePoolManagerComponent;
class USMTileManagerComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRoundStateChangedDelegate);

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

	USMTileManagerComponent* GetTileManager() const { return TileManager; }

	USMProjectilePoolManagerComponent* GetProjectilePoolManager() const { return ProjectilePoolManager; }

	ESMRoundState GetRoundState() const { return RoundState; }

	void SetRoundState(ESMRoundState NewRoundState);

	UPROPERTY(BlueprintAssignable)
	FOnRoundStateChangedDelegate OnPreRoundStart;

	UPROPERTY(BlueprintAssignable)
	FOnRoundStateChangedDelegate OnInRoundStart;

	UPROPERTY(BlueprintAssignable)
	FOnRoundStateChangedDelegate OnPostRoundStart;

protected:
	virtual void HandleMatchIsWaitingToStart() override;

	UFUNCTION()
	void OnRep_RoundState();

	UPROPERTY(VisibleAnywhere, Category = "Design")
	TObjectPtr<USMRoundTimerManagerComponent> RoundTimerManager;

	UPROPERTY(VisibleAnywhere, Category = "Design")
	TObjectPtr<USMTileManagerComponent> TileManager;

	UPROPERTY(VisibleAnywhere, Category = "Design")
	TObjectPtr<USMScoreMusicManagerComponent> ScoreMusicManager;

	UPROPERTY(VisibleAnywhere, Category = "Design")
	TObjectPtr<USMProjectilePoolManagerComponent> ProjectilePoolManager;

	UPROPERTY(ReplicatedUsing = "OnRep_RoundState")
	ESMRoundState RoundState = ESMRoundState::None;
};
