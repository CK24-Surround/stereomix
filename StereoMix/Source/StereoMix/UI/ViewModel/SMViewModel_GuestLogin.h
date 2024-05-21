// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMViewModel.h"
#include "SMViewModel_GuestLogin.generated.h"

/**
 * GuestLogin ViewModel
 */
UCLASS(BlueprintType)
class STEREOMIX_API USMViewModel_GuestLogin : public USMViewModel
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, meta=(AllowPrivateAccess))
	FText UserName;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, meta=(AllowPrivateAccess))
	bool SubmitButtonEnabled{true};

public:
	FText GetUserName() const { return UserName; }
	void SetUserName(const FText& NewUserName) { UE_MVVM_SET_PROPERTY_VALUE(UserName, NewUserName); }

	bool GetSubmitButtonEnabled() const { return SubmitButtonEnabled; }
	void SetSubmitButtonEnabled(const bool NewSubmitButtonEnabled) { UE_MVVM_SET_PROPERTY_VALUE(SubmitButtonEnabled, NewSubmitButtonEnabled); }

	UFUNCTION(BlueprintCallable, Category="UI")
	void Submit();
};
