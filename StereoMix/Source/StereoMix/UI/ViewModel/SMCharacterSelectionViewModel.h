// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "SMViewModel.h"
#include "SMCharacterSelectionEntryViewModel.h"
#include "Data/SMCharacterType.h"
#include "Data/SMTeam.h"
#include "SMCharacterSelectionViewModel.generated.h"

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
class STEREOMIX_API USMCharacterSelectionViewModel : public USMViewModel
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, FieldNotify, meta=(AllowPrivateAccess))
	ESMTeam LocalPlayerTeam;
	
	UPROPERTY(BlueprintReadWrite, FieldNotify, meta=(AllowPrivateAccess))
	float DefaultTime;
	
	UPROPERTY(BlueprintReadWrite, FieldNotify, meta=(AllowPrivateAccess))
	float RemainingTime;

	UPROPERTY(BlueprintReadOnly, FieldNotify, meta=(AllowPrivateAccess))
	TArray<TObjectPtr<USMCharacterSelectionEntryViewModel>> PlayerEntries;

	TWeakObjectPtr<ASMCharacterSelectState> OwningGameState;
	TWeakObjectPtr<ASMCharacterSelectPlayerState> LocalPlayerState;

public:
	virtual void InitializeViewModel(UWorld* InWorld) override;

protected:
	float GetDefaultTime() const { return DefaultTime; }
	void SetDefaultTime(float NewDefaultTime) { UE_MVVM_SET_PROPERTY_VALUE(DefaultTime, NewDefaultTime); }
	
	float GetRemainingTime() const { return RemainingTime; }
	void SetRemainingTime(float NewRemainingTime) { UE_MVVM_SET_PROPERTY_VALUE(RemainingTime, NewRemainingTime); }
	
	UFUNCTION()
	void OnCountdownTick();	
};
