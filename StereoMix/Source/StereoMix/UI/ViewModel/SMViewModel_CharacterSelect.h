// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "SMViewModel.h"
#include "SMViewModel_CharacterSelect.generated.h"

class ASMCharacterSelectPlayerState;
enum class ESMTeam : uint8;
class ASMCharacterSelectState;
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

	TWeakObjectPtr<ASMCharacterSelectState> OwningGameState;
	TWeakObjectPtr<ASMCharacterSelectPlayerState> LocalPlayerState;

public:
	void BindGameState(ASMCharacterSelectState* GameState);

protected:
	int32 GetRemainingTime() const { return RemainingTime; }
	void SetRemainingTime(int32 NewRemainingTime) { UE_MVVM_SET_PROPERTY_VALUE(RemainingTime, NewRemainingTime); }
	
	UFUNCTION()
	void OnCountdownTick();	
};
