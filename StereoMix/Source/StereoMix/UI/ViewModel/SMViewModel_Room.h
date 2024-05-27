// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMViewModel.h"
#include "SMViewModel_RoomPlayer.h"
#include "SMViewModel_RoomTeam.h"
#include "Connection/SMClientConnectionSubsystem.h"
#include "Games/SMRoomState.h"
#include "SMViewModel_Room.generated.h"

UENUM(BlueprintType)
enum class ERoomUiState : uint8
{
	NotReady,
	Ready,
	Countdown,
	Traveling
};

UCLASS(BlueprintType)
class STEREOMIX_API URoomPlayerData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	bool bEmpty;

	UPROPERTY(BlueprintReadOnly)
	FString UserName;

	UPROPERTY(BlueprintReadOnly)
	bool bLocalPlayer;
};

/**
 * 
 */
UCLASS(BlueprintType)
class STEREOMIX_API USMViewModel_Room : public USMViewModel
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, FieldNotify, meta=(AllowPrivateAccess))
	ERoomUiState CurrentUiState;
	
	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter=GetRoomCodeText, meta=(AllowPrivateAccess))
	FText RoomCodeText;

	UPROPERTY(BlueprintReadWrite, FieldNotify, meta=(AllowPrivateAccess))
	int32 CurrentPlayerCount;
	
	UPROPERTY(BlueprintReadWrite, FieldNotify, meta=(AllowPrivateAccess))
	TObjectPtr<USMViewModel_RoomTeam> TeamA;

	UPROPERTY(BlueprintReadWrite, FieldNotify, meta=(AllowPrivateAccess))
	TObjectPtr<USMViewModel_RoomTeam> TeamB;

	TWeakObjectPtr<ASMRoomState> OwningRoomState;
	TWeakObjectPtr<USMClientConnectionSubsystem> ConnectionSubsystem;
	
public:
	USMViewModel_Room();

	void BindGameState(ASMRoomState* InRoomState);

	const FText& GetRoomCodeText() const { return RoomCodeText; }
	void SetRoomCodeText(const FText& NewRoomCodeText) { UE_MVVM_SET_PROPERTY_VALUE(RoomCodeText, NewRoomCodeText); }

	int32 GetPlayerCount() const { return CurrentPlayerCount; }
	void SetPlayerCount(const int32 PlayerCount) { UE_MVVM_SET_PROPERTY_VALUE(CurrentPlayerCount, PlayerCount); }
	
	void SetTeamA(USMViewModel_RoomTeam* NewTeamA) { UE_MVVM_SET_PROPERTY_VALUE(TeamA, NewTeamA); }
	void SetTeamB(USMViewModel_RoomTeam* NewTeamB) { UE_MVVM_SET_PROPERTY_VALUE(TeamB, NewTeamB); }

	UFUNCTION(BlueprintCallable)
	void CopyCode();
	
	UFUNCTION(BlueprintCallable)
	void QuitRoom();
	
	UFUNCTION()
	void OnPlayerJoined(ASMPlayerState* JoinedPlayer);
	
	UFUNCTION()
	void OnPlayerLeft(ASMPlayerState* LeftPlayer);	
};
