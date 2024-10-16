// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Games/SMGameStateNotify.h"

#include "SMRoomState.generated.h"


enum class ESMTeam : uint8;
class ASMRoomPlayerState;
class USMCountdownTimerComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTeamPlayersUpdated, ESMTeam, UpdatedTeam);

/**
 * StereoMix Room State
 */
UCLASS()
class STEREOMIX_API ASMRoomState : public AGameStateBase, public ISMGameStateNotify
{
	GENERATED_BODY()

public:
	ASMRoomState();

	virtual void PostInitializeComponents() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void AddPlayerState(APlayerState* PlayerState) override;

	virtual void RemovePlayerState(APlayerState* PlayerState) override;

	USMCountdownTimerComponent* GetCountdownTimer() const { return CountdownTimer.Get(); }

	UFUNCTION(BlueprintCallable)
	TArray<ASMRoomPlayerState*> GetTeamEdmPlayers() { return TeamEdmPlayers; }

	UFUNCTION(BlueprintCallable)
	TArray<ASMRoomPlayerState*> GetTeamFutureBassPlayers() { return TeamFutureBassPlayers; }

	UFUNCTION(BlueprintCallable)
	TArray<ASMRoomPlayerState*> GetPlayersInTeam(ESMTeam Team) const;

	UFUNCTION(BlueprintPure)
	const FString& GetRoomName() const { return RoomName; }

	void SetRoomName(const FString& NewRoomName) { RoomName = NewRoomName; }

	UFUNCTION(BlueprintPure)
	const FString& GetRoomCode() const { return RoomCode; }

	void SetRoomCode(const FString& NewRoomCode) { RoomCode = NewRoomCode; }

	virtual void NotifyPlayerJoined(ASMPlayerState* JoinedPlayer) override;

	virtual void NotifyPlayerLeft(ASMPlayerState* LeftPlayer) override;

	virtual void NotifyPlayerTeamChanged(ASMPlayerState* Player, ESMTeam PreviousTeam, ESMTeam NewTeam) override;

	virtual void NotifyPlayerCharacterChanged(ASMPlayerState* Player, ESMCharacterType NewCharacter) override
	{
	}

	UPROPERTY(BlueprintAssignable)
	FPlayerJoined OnPlayerJoined;

	UPROPERTY(BlueprintAssignable)
	FPlayerLeft OnPlayerLeft;

	UPROPERTY(BlueprintAssignable)
	FTeamPlayersUpdated OnTeamPlayersUpdated;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated)
	int32 MaxPlayersInGame = 2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated)
	int32 MaxPlayersInTeam = 1;

	UPROPERTY(EditDefaultsOnly)
	int32 CountdownTime;

protected:
	UFUNCTION()
	virtual void OnRep_RoomName();

	UFUNCTION()
	virtual void OnRep_RoomCode();

	UFUNCTION()
	virtual void OnRep_TeamEdmPlayers();

	UFUNCTION()
	virtual void OnRep_TeamFutureBassPlayers();

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_TeamEdmPlayers)
	TArray<TObjectPtr<ASMRoomPlayerState>> TeamEdmPlayers;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_TeamFutureBassPlayers)
	TArray<TObjectPtr<ASMRoomPlayerState>> TeamFutureBassPlayers;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_RoomName)
	FString RoomName;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_RoomCode)
	FString RoomCode;

private:
	UPROPERTY()
	TObjectPtr<USMCountdownTimerComponent> CountdownTimer;
};
