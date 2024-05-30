// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "SMViewModel.h"
#include "SMViewModel_CharacterSelect_Player.h"
#include "Data/SMCharacterType.h"
#include "Data/SMTeam.h"
#include "SMViewModel_CharacterSelect.generated.h"

class ASMCharacterSelectState;
class ASMCharacterSelectPlayerState;

UCLASS(BlueprintType)
class STEREOMIX_API USMCharacterSelectPlayerData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	bool bLocalPlayer;

	UPROPERTY(BlueprintReadOnly)
	FText PlayerName;

	UPROPERTY(BlueprintReadOnly)
	ESMCharacterType CharacterType;
};

/**
 * 
 */
UCLASS(BlueprintType)
class STEREOMIX_API USMViewModel_CharacterSelect : public USMViewModel
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, FieldNotify, meta=(AllowPrivateAccess))
	ESMTeam LocalPlayerTeam;
	
	UPROPERTY(BlueprintReadWrite, FieldNotify, meta=(AllowPrivateAccess))
	int32 TimerTime;
	
	UPROPERTY(BlueprintReadWrite, FieldNotify, meta=(AllowPrivateAccess))
	int32 RemainingTime;

	UPROPERTY(BlueprintReadOnly, FieldNotify, meta=(AllowPrivateAccess))
	TArray<TObjectPtr<USMViewModel_CharacterSelect_Player>> PlayerEntries;

	TWeakObjectPtr<ASMCharacterSelectState> OwningGameState;
	TWeakObjectPtr<ASMCharacterSelectPlayerState> LocalPlayerState;

public:
	virtual void InitializeViewModel(UWorld* InWorld) override;

protected:
	int32 GetRemainingTime() const { return RemainingTime; }
	void SetRemainingTime(int32 NewRemainingTime) { UE_MVVM_SET_PROPERTY_VALUE(RemainingTime, NewRemainingTime); }
	
	UFUNCTION()
	void OnCountdownTick();	
};
