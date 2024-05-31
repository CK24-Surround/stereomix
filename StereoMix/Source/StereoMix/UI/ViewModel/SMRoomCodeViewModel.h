// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMViewModel.h"
#include "StereoMix.h"
#include "SMRoomCodeViewModel.generated.h"

/**
 * Room Short Code Popup ViewModel
 */
UCLASS(BlueprintType)
class STEREOMIX_API USMRoomCodeViewModel : public USMViewModel
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter=SetRoomShortCodeText, Getter=GetRoomShortCodeText, meta=(AllowPrivateAccess))
	FText RoomShortCodeText;

	UPROPERTY(BlueprintReadWrite, FieldNotify, meta=(AllowPrivateAccess))
	bool bSubmitButtonEnabled = false;

	UPROPERTY(BlueprintReadWrite, FieldNotify, meta=(AllowPrivateAccess))
	bool bCancelButtonEnabled = true;

public:
	FSimpleDelegate OnSubmit;
	FSimpleDelegate OnCancel;

	FText GetRoomShortCodeText() const
	{
		return RoomShortCodeText;
	}

	void SetRoomShortCodeText(const FText& NewRoomShortCode)
	{
		if (UE_MVVM_SET_PROPERTY_VALUE(RoomShortCodeText, NewRoomShortCode))
		{
			SetSubmitButtonEnabled(IsValidRoomShortCode(NewRoomShortCode));
		}
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
