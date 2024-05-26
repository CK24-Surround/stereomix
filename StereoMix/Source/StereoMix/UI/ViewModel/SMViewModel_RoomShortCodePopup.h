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

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter=SetRoomShortCodeText, meta=(AllowPrivateAccess))
	FText RoomShortCodeText;

	UPROPERTY(BlueprintReadWrite, FieldNotify, meta=(AllowPrivateAccess))
	bool bSubmitButtonEnabled;

	UPROPERTY(BlueprintReadWrite, FieldNotify, meta=(AllowPrivateAccess))
	bool bCancelButtonEnabled;

public:
	FSimpleDelegate OnSubmit;
	FSimpleDelegate OnCancel;

	FText GetRoomShortCodeText() const { return RoomShortCodeText; }

	void SetRoomShortCodeText(const FText& NewRoomShortCode)
	{
		UE_MVVM_SET_PROPERTY_VALUE(RoomShortCodeText, NewRoomShortCode.ToUpper());
		SetSubmitButtonEnabled(IsValidRoomShortCode(NewRoomShortCode));
	}

	bool IsSubmitButtonEnabled() const { return bSubmitButtonEnabled; }
	void SetSubmitButtonEnabled(const bool bEnabled) { UE_MVVM_SET_PROPERTY_VALUE(bSubmitButtonEnabled, bEnabled); }

	bool IsCancelButtonEnabled() const { return bCancelButtonEnabled; }
	void SetCancelButtonEnabled(const bool bEnabled) { UE_MVVM_SET_PROPERTY_VALUE(bCancelButtonEnabled, bEnabled); }

	UFUNCTION(BlueprintCallable, Category="UI")
	void Submit();

	UFUNCTION(BlueprintCallable, Category="UI")
	void Cancel();

protected:
	static bool IsValidRoomShortCode(const FText& RoomShortCodeToCheck);
};
