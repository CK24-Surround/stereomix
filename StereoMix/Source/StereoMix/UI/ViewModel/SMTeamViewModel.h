// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMViewModel.h"
#include "SMRoomPlayerViewModel.h"
#include "Data/SMTeam.h"
#include "Games/SMRoomState.h"
#include "SMTeamViewModel.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class STEREOMIX_API USMTeamViewModel : public USMViewModel
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, FieldNotify, meta=(AllowPrivateAccess))
	TObjectPtr<USMRoomPlayerViewModel> Player1;

	UPROPERTY(BlueprintReadWrite, FieldNotify, meta=(AllowPrivateAccess))
	TObjectPtr<USMRoomPlayerViewModel> Player2;

	UPROPERTY(BlueprintReadWrite, FieldNotify, meta=(AllowPrivateAccess))
	TObjectPtr<USMRoomPlayerViewModel> Player3;

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
	TArray<TObjectPtr<USMRoomPlayerViewModel>> Players;

	USMTeamViewModel();

	const TArray<TObjectPtr<USMRoomPlayerViewModel>>& GetPlayers() const { return Players; }

	void BindGameState(ASMRoomState* InRoomState);

	ESMTeam GetTeam() const { return Team; }
	bool IsIncludeLocalPlayer() const { return bIncludeLocalPlayer; }

	void SetTeam(const ESMTeam NewTeam) { UE_MVVM_SET_PROPERTY_VALUE(Team, NewTeam); }

	void SetPlayer1(USMRoomPlayerViewModel* NewPlayer1) { UE_MVVM_SET_PROPERTY_VALUE(Player1, NewPlayer1); }
	void SetPlayer2(USMRoomPlayerViewModel* NewPlayer2) { UE_MVVM_SET_PROPERTY_VALUE(Player2, NewPlayer2); }
	void SetPlayer3(USMRoomPlayerViewModel* NewPlayer3) { UE_MVVM_SET_PROPERTY_VALUE(Player3, NewPlayer3); }

	UFUNCTION(BlueprintCallable)
	void ChangeTeam();

protected:
	void SetTeamFull(const bool bFull) { UE_MVVM_SET_PROPERTY_VALUE(bIsFull, bFull); }

	void SetIncludeLocalPlayer(const bool bInclude) { UE_MVVM_SET_PROPERTY_VALUE(bIncludeLocalPlayer, bInclude); }
	
	UFUNCTION()
	void OnTeamPlayersUpdated(ESMTeam UpdatedTeam);
};
