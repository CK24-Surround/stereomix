// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGameStateNotify.h"
#include "GameFramework/GameStateBase.h"
#include "SMCharacterSelectState.generated.h"

enum class ESMCharacterType : uint8;
class UCountdownTimerComponent;

UENUM(BlueprintType)
enum class ECharacterSelectionState : uint8
{
	Select,
	End
};

DECLARE_EVENT_OneParam(ASMCharacterSelectState, FCharacterSelectionStateChangedEvent, ECharacterSelectionState /* NewCharacterSelectionState */)

/**
 * 
 */
UCLASS()
class STEREOMIX_API ASMCharacterSelectState : public AGameStateBase, public ISMGameStateNotify
{

private:
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UCountdownTimerComponent> CountdownTimer;

	UPROPERTY(ReplicatedUsing=OnRep_CharacterSelectionState)
	ECharacterSelectionState CharacterSelectionState;

	UFUNCTION()
	void OnRep_CharacterSelectionState() const;
	
public:
	UPROPERTY(EditDefaultsOnly)
	int32 CountdownTime;
	
	FPlayerCharacterChanged OnPlayerCharacterChanged;
	FCharacterSelectionStateChangedEvent CharacterSelectionStateChangedEvent;
	
	ASMCharacterSelectState();

	virtual void AddPlayerState(APlayerState* PlayerState) override;

	virtual void RemovePlayerState(APlayerState* PlayerState) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UCountdownTimerComponent* GetCountdownTimer() const { return CountdownTimer.Get(); }

	void SetCharacterSelectionState(ECharacterSelectionState NewCharacterSelectionState);
	
	virtual void NotifyPlayerJoined(ASMPlayerState* JoinedPlayer) override;
	virtual void NotifyPlayerLeft(ASMPlayerState* LeftPlayer) override;
	virtual void NotifyPlayerTeamChanged(ASMPlayerState* Player, ESMTeam PreviousTeam, ESMTeam NewTeam) override;
	virtual void NotifyPlayerCharacterChanged(ASMPlayerState* Player, ESMCharacterType NewCharacter) override;
};
