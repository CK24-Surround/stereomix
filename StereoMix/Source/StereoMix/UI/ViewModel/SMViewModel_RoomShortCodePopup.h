// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMViewModel.h"
#include "SMViewModel_RoomShortCodePopup.generated.h"

/**
 * Room Short Code Popup ViewModel
 */
UCLASS()
class STEREOMIX_API USMViewModel_RoomShortCodePopup : public USMViewModel
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, meta=(AllowPrivateAccess))
	FText RoomShortCode;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, meta=(AllowPrivateAccess))
	bool IsEnterRoomButtonEnabled;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, meta=(AllowPrivateAccess))
	bool IsCancelButtonEnabled;

public:
	FText GetRoomShortCode() const { return RoomShortCode; }
	void SetRoomShortCode(const FText& NewRoomShortCode)
	{
		if (!NewRoomShortCode.IsEmptyOrWhitespace() && UE_MVVM_SET_PROPERTY_VALUE(RoomShortCode, NewRoomShortCode))
		{
			SetIsEnterRoomButtonEnabled(true);
		}
		else
		{
			SetIsEnterRoomButtonEnabled(false);
		}
	}

	bool GetIsEnterRoomButtonEnabled() const { return IsEnterRoomButtonEnabled; }
	void SetIsEnterRoomButtonEnabled(const bool NewIsEnterRoomButtonEnabled) { UE_MVVM_SET_PROPERTY_VALUE(IsEnterRoomButtonEnabled, NewIsEnterRoomButtonEnabled); }

	bool GetIsCancelButtonEnabled() const { return IsCancelButtonEnabled; }
	void SetIsCancelButtonEnabled(const bool NewIsCancelButtonEnabled) { UE_MVVM_SET_PROPERTY_VALUE(IsCancelButtonEnabled, NewIsCancelButtonEnabled); }

	UFUNCTION(BlueprintCallable)
	void EnterRoom();

	UFUNCTION(BlueprintCallable)
	void Cancel();
};
