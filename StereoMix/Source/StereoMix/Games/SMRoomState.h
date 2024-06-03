// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGameStateNotify.h"
#include "GameFramework/GameStateBase.h"
#include "SMRoomState.generated.h"

enum class ESMTeam : uint8;
class ASMRoomPlayerState;
class UCountdownTimerComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTeamPlayersUpdated, ESMTeam, UpdatedTeam);

/**
 * StereoMix Room State
 */
UCLASS()
class STEREOMIX_API ASMRoomState : public AGameStateBase, public ISMGameStateNotify
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UCountdownTimerComponent> CountdownTimer;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_UnreadyPlayers, meta=(AllowPrivateAccess=true))
	TArray<TObjectPtr<ASMRoomPlayerState>> UnreadyPlayers;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_TeamEdmPlayers, meta=(AllowPrivateAccess=true))
	TArray<TObjectPtr<ASMRoomPlayerState>> TeamEdmPlayers;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_TeamFutureBassPlayers, meta=(AllowPrivateAccess=true))
	TArray<TObjectPtr<ASMRoomPlayerState>> TeamFutureBassPlayers;

	UFUNCTION()
	void OnRep_UnreadyPlayers() const;

	UFUNCTION()
	void OnRep_TeamEdmPlayers() const;

	UFUNCTION()
	void OnRep_TeamFutureBassPlayers() const;

public:
	UPROPERTY(BlueprintAssignable)
	FPlayerJoined OnPlayerJoined;

	UPROPERTY(BlueprintAssignable)
	FPlayerLeft OnPlayerLeft;

	UPROPERTY(BlueprintAssignable)
	FTeamPlayersUpdated OnTeamPlayersUpdated;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated)
	int32 ReplicatedMaxPlayersInGame;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated)
	int32 ReplicatedMaxPlayersInTeam;

	UPROPERTY(EditDefaultsOnly)
	int32 CountdownTime;

	UPROPERTY(BlueprintReadOnly, Replicated)
	FString ShortRoomCode;

	ASMRoomState();

	virtual void AddPlayerState(APlayerState* PlayerState) override;

	virtual void RemovePlayerState(APlayerState* PlayerState) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UCountdownTimerComponent* GetCountdownTimer() const { return CountdownTimer.Get(); }

	UFUNCTION(BlueprintCallable)
	TArray<ASMRoomPlayerState*> GetTeamEdmPlayers() const { return TeamEdmPlayers; }

	UFUNCTION(BlueprintCallable)
	TArray<ASMRoomPlayerState*> GetTeamFutureBassPlayers() const { return TeamFutureBassPlayers; }

	UFUNCTION(BlueprintCallable)
	TArray<ASMRoomPlayerState*> GetTeamPlayers(ESMTeam Team) const;

public:
	virtual void NotifyPlayerJoined(ASMPlayerState* JoinedPlayer) override;
	virtual void NotifyPlayerLeft(ASMPlayerState* LeftPlayer) override;
	virtual void NotifyPlayerTeamChanged(ASMPlayerState* Player, ESMTeam PreviousTeam, ESMTeam NewTeam) override;
	virtual void NotifyPlayerCharacterChanged(ASMPlayerState* Player, ESMCharacterType NewCharacter) override;
};
