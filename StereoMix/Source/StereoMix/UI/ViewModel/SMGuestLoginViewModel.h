// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMLoginViewModel.h"
#include "SMGuestLoginViewModel.generated.h"

/**
 * GuestLogin ViewModel
 */
UCLASS(BlueprintType)
class STEREOMIX_API USMGuestLoginViewModel : public USMLoginViewModel
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter=SetInputUserName, meta=(AllowPrivateAccess))
	FText InputUserName;

public:
	const FText& GetUserName() const { return InputUserName; }

protected:
	static bool IsValidUserName(const FText& UserNameToCheck);
	
	void SetInputUserName(const FText& UserName);

	virtual void OnSubmit() override;
};
