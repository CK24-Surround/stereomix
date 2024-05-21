// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMViewModel.h"
#include "StereoMix.h"
#include "SMViewModel_Lobby.generated.h"

/**
 * Lobby ViewModel
 */
UCLASS()
class STEREOMIX_API USMViewModel_Lobby : public USMViewModel
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	bool IsAllButtonEnabled{true};

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	FText Status;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	bool DisplayStatus;
	
public:
	// ==============================================================
	// View

	bool GetIsAllButtonEnabled() const { return IsAllButtonEnabled; }
	void SetIsAllButtonEnabled(const bool NewIsAllButtonEnabled) { UE_MVVM_SET_PROPERTY_VALUE(IsAllButtonEnabled, NewIsAllButtonEnabled); }

	const FText& GetStatus() const { return Status; }
	void SetStatus(const FText& NewStatus)
	{
		UE_LOG(LogStereoMix, Verbose, TEXT("NewStatus: %s"), *NewStatus.ToString())
		if (UE_MVVM_SET_PROPERTY_VALUE(Status, NewStatus))
		{
			SetDisplayStatus(!NewStatus.IsEmptyOrWhitespace());
		}
	}

	bool GetDisplayStatus() const { return DisplayStatus; }
	void SetDisplayStatus(const bool NewVisibility)
	{
		UE_MVVM_SET_PROPERTY_VALUE(DisplayStatus, NewVisibility);
	}
	
	UFUNCTION(BlueprintCallable, Category="UI")
	void CreateRoom();

	UFUNCTION(BlueprintCallable, Category="UI")
	void QuickMatch();

	UFUNCTION(BlueprintCallable, Category="UI")
	void JoinRoom();

	UFUNCTION(BlueprintCallable, Category="UI")
	void Back();
};
