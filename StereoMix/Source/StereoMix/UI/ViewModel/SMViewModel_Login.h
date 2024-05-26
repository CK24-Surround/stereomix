// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMViewModel.h"
#include "Components/Button.h"
#include "SMViewModel_Login.generated.h"


// DECLARE_DYNAMIC_DELEGATE(FOnClicked)

/**
 * Default Login ViewModel
 */
UCLASS(BlueprintType)
class STEREOMIX_API USMViewModel_Login : public USMViewModel
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, FieldNotify, meta=(AllowPrivateAccess))
	bool bSubmitButtonEnabled;

public:
	USMViewModel_Login();

	FSimpleDelegate OnSubmit;

	bool GetSubmitButtonEnabled() const { return bSubmitButtonEnabled; }

	UFUNCTION(BlueprintCallable, Category="UI")
	virtual void Submit();

protected:
	void SetSubmitButtonEnabled(const bool bEnabled) { UE_MVVM_SET_PROPERTY_VALUE(bSubmitButtonEnabled, bEnabled); }
};
