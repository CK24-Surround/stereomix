// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMViewModel.h"
#include "Player/SMRoomPlayerState.h"
#include "SMViewModel_RoomPlayer.generated.h"

/**
 * Room Player ViewModel
 */
UCLASS(BlueprintType)
class STEREOMIX_API USMViewModel_RoomPlayer : public USMViewModel
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter=SetIsEmpty, Getter=IsEmpty, meta=(AllowPrivateAccess))
	bool bEmpty = true;

	UPROPERTY(BlueprintReadWrite, FieldNotify, meta=(AllowPrivateAccess))
	FText PlayerNameText;

	UPROPERTY(BlueprintReadWrite, FieldNotify, meta=(AllowPrivateAccess))
	bool bLocalPlayer = false;

	TWeakObjectPtr<ASMRoomPlayerState> OwningPlayerState;

public:
	USMViewModel_RoomPlayer();

	bool IsEmpty() const { return bEmpty; }
	void SetIsEmpty(const bool bNewValue) { UE_MVVM_SET_PROPERTY_VALUE(bEmpty, bNewValue); }
	
	const FText& GetPlayerNameText() const { return PlayerNameText; }
	bool IsLocalPlayer() const { return bLocalPlayer; }

	void Init(ASMRoomPlayerState* TargetPlayer);

	void Remove();

	void SetItem(const FText& PlayerName, const bool bIsLocalPlayer);

	void ResetItem();

	ASMRoomPlayerState* GetTargetPlayer() const { return OwningPlayerState.IsValid() ? OwningPlayerState.Get() : nullptr; }
};
