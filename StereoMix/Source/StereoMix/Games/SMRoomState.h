// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/SMTeam.h"
#include "GameFramework/GameStateBase.h"
#include "Player/SMRoomPlayerState.h"
#include "SMRoomState.generated.h"

DECLARE_EVENT_OneParam(ASMRoomState, FPlayerAddEvent, APlayerState* /*AddedPlayerState*/)
DECLARE_EVENT_OneParam(ASMRoomState, FPlayerRemoveEvent, APlayerState* /*RemovedPlayerState*/)
DECLARE_EVENT_TwoParams(ASMRoomState, FPlayerTeamChangeEvent, APlayerState* /*PlayerState*/, ESMTeam /*NewTeam*/)

/**
 * StereoMix Room State
 */
UCLASS()
class STEREOMIX_API ASMRoomState : public AGameStateBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	int32 MaxPlayersInTeam;

	ASMRoomState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	// ============================================================================
	// Countdown
public:
	FPlayerAddEvent PlayerAddEvent;
	FPlayerRemoveEvent PlayerRemoveEvent;
	FPlayerTeamChangeEvent PlayerTeamChangeEvent;

	UPROPERTY(ReplicatedUsing = OnRep_Countdown)
	int8 Countdown = 0;

	UFUNCTION()
	void OnRep_Countdown() const;

	UFUNCTION(Reliable, Client)
	void StartCountdown();

	UFUNCTION(Reliable, Client)
	void CancelCountdown();


	// ============================================================================
	// Team
	
public:
	int32 GetPlayerCountInTeam(ESMTeam Team) const;


	// ============================================================================
	// Player State

public:
	void BroadcastPlayerAdded(APlayerState* PlayerState) const;
	void BroadcastPlayerRemoved(APlayerState* PlayerState) const;
	void BroadcastTeamChanged(ASMRoomPlayerState* TargetPlayerState, ESMTeam NewTeam) const;
};
