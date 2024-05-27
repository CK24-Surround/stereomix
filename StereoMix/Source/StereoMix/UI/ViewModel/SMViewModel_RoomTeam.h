// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMViewModel.h"
#include "SMViewModel_RoomPlayer.h"
#include "Data/SMTeam.h"
#include "Games/SMRoomState.h"
#include "SMViewModel_RoomTeam.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class STEREOMIX_API USMViewModel_RoomTeam : public USMViewModel
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, FieldNotify, meta=(AllowPrivateAccess))
	TObjectPtr<USMViewModel_RoomPlayer> Player1;

	UPROPERTY(BlueprintReadWrite, FieldNotify, meta=(AllowPrivateAccess))
	TObjectPtr<USMViewModel_RoomPlayer> Player2;

	UPROPERTY(BlueprintReadWrite, FieldNotify, meta=(AllowPrivateAccess))
	TObjectPtr<USMViewModel_RoomPlayer> Player3;

	UPROPERTY(BlueprintReadWrite, FieldNotify, meta=(AllowPrivateAccess))
	ESMTeam Team;

	UPROPERTY(BlueprintReadWrite, FieldNotify, meta=(AllowPrivateAccess))
	bool bIsFull = false;

	UPROPERTY(BlueprintReadWrite, FieldNotify, meta=(AllowPrivateAccess))
	bool bIncludeLocalPlayer = false;

	TWeakObjectPtr<ASMRoomState> OwningRoomState;

	bool AddPlayer(ASMRoomPlayerState* Player);
	bool RemovePlayer(const ASMRoomPlayerState* Player);

public:
	UPROPERTY()
	TArray<TObjectPtr<USMViewModel_RoomPlayer>> Players;

	USMViewModel_RoomTeam();

	const TArray<TObjectPtr<USMViewModel_RoomPlayer>>& GetPlayers() const { return Players; }

	void BindGameState(ASMRoomState* InRoomState);

	ESMTeam GetTeam() const { return Team; }
	bool IsIncludeLocalPlayer() const { return bIncludeLocalPlayer; }

	void SetTeam(const ESMTeam NewTeam) { UE_MVVM_SET_PROPERTY_VALUE(Team, NewTeam); }

	void SetPlayer1(USMViewModel_RoomPlayer* NewPlayer1) { UE_MVVM_SET_PROPERTY_VALUE(Player1, NewPlayer1); }
	void SetPlayer2(USMViewModel_RoomPlayer* NewPlayer2) { UE_MVVM_SET_PROPERTY_VALUE(Player2, NewPlayer2); }
	void SetPlayer3(USMViewModel_RoomPlayer* NewPlayer3) { UE_MVVM_SET_PROPERTY_VALUE(Player3, NewPlayer3); }

	UFUNCTION(BlueprintCallable)
	void ChangeTeam();

protected:
	void SetTeamFull(const bool bFull) { UE_MVVM_SET_PROPERTY_VALUE(bIsFull, bFull); }

	void SetIncludeLocalPlayer(const bool bInclude) { UE_MVVM_SET_PROPERTY_VALUE(bIncludeLocalPlayer, bInclude); }
	
	UFUNCTION()
	void OnTeamPlayersUpdated(ESMTeam UpdatedTeam);
};
