// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Games/SMPlayerState.h"
#include "SMRoomState.h"

#include "SMRoomPlayerState.generated.h"

UENUM(BlueprintType)
enum class ERoomPlayerStateType : uint8
{
	Loading,
	Unready,
	Ready
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoomPlayerStateTypeChanged, ERoomPlayerStateType, NewState);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTeamChangeResponse, bool, bSuccess, ESMTeam, NewTeam);

/**
 *
 */
UCLASS()
class STEREOMIX_API ASMRoomPlayerState : public ASMPlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnRoomPlayerStateTypeChanged OnCurrentStateChanged;

	FOnTeamChangeResponse OnTeamChangeResponse;

	ASMRoomPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ChangeTeam(ESMTeam NewTeam);

	UFUNCTION(Client, Reliable)
	void ResponseChangeTeam(bool bSuccess, ESMTeam NewTeam);

	ASMRoomState* GetRoomState() const { return RoomState.Get(); }

	ERoomPlayerStateType GetCurrentState() const { return CurrentState; }

	UFUNCTION(Reliable, Server)
	void SetCurrentState(ERoomPlayerStateType NewState);

protected:
	UPROPERTY(Transient)
	TWeakObjectPtr<ASMRoomState> RoomState;

	UPROPERTY(VisibleInstanceOnly, ReplicatedUsing = OnRep_CurrentState)
	ERoomPlayerStateType CurrentState;

	UFUNCTION()
	virtual void OnRep_CurrentState();

	virtual bool CanChangeTeam(ESMTeam NewTeam) const override;

	virtual void OnTeamChanged(ESMTeam PreviousTeam, ESMTeam NewTeam) override;
};
