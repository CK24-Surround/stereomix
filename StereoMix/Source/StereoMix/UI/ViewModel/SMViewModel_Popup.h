// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMViewModel.h"
#include "SMViewModel_Popup.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnPopupClosed);

/**
 * 
 */
UCLASS(BlueprintType)
class STEREOMIX_API USMViewModel_Popup : public USMViewModel
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, meta=(AllowPrivateAccess))
	FText Content;

public:
	FOnPopupClosed OnPopupClosed;

	const FText& GetContent() const { return Content; }
	void SetContent(const FText& NewContent) { UE_MVVM_SET_PROPERTY_VALUE(Content, NewContent); }

	UFUNCTION(BlueprintCallable, Category="UI")
	void Close() const;
};
