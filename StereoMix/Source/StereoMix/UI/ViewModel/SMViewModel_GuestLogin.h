// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMViewModel_Login.h"
#include "StereoMix.h"
#include "SMViewModel_GuestLogin.generated.h"

/**
 * GuestLogin ViewModel
 */
UCLASS(BlueprintType)
class STEREOMIX_API USMViewModel_GuestLogin : public USMViewModel_Login
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter=SetUserNameText, meta=(AllowPrivateAccess))
	FText UserNameText;

public:
	FText GetUserName() const
	{
		return UserNameText;
	}

	virtual void Submit() override;

protected:
	void SetUserNameText(const FText& UserName)
	{
		UE_MVVM_SET_PROPERTY_VALUE(UserNameText, UserName);
		if (IsValidUserName(UserName))
		{
			SetSubmitButtonEnabled(true);
		}
		else
		{
			SetSubmitButtonEnabled(false);
		}
	}

	static bool IsValidUserName(const FText& UserNameToCheck);
};
