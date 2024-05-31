// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMViewModel.h"
#include "SMCharacterSelectionEntryViewModel.generated.h"

enum class ESMCharacterType : uint8;
class ASMPlayerState;
/**
 * 
 */
UCLASS(BlueprintType)
class STEREOMIX_API USMCharacterSelectionEntryViewModel : public USMViewModel
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Setter=SetBind, Getter=IsBind, FieldNotify, meta=(AllowPrivateAccess))
	bool bBind;

	UPROPERTY(BlueprintReadWrite, FieldNotify, meta=(AllowPrivateAccess))
	FText PlayerName;

	UPROPERTY(BlueprintReadWrite, FieldNotify, meta=(AllowPrivateAccess))
	ESMCharacterType CharacterType;

	TWeakObjectPtr<ASMPlayerState> OwningPlayerState;

public:
	virtual void InitializeViewModel(UWorld* InWorld) override;
	
	void BindPlayerState(ASMPlayerState* PlayerState);

	void Reset();

	UFUNCTION()
	void SetCharacterType(ESMCharacterType NewCharacterType) { UE_MVVM_SET_PROPERTY_VALUE(CharacterType, NewCharacterType); }

protected:
	bool IsBind() const { return bBind; }
	void SetBind(const bool bNewBind) { UE_MVVM_SET_PROPERTY_VALUE(bBind, bNewBind); }

	const FText& GetPlayerName() const { return PlayerName; }
	void SetPlayerName(const FText& NewPlayerName) { UE_MVVM_SET_PROPERTY_VALUE(PlayerName, NewPlayerName); }
};
