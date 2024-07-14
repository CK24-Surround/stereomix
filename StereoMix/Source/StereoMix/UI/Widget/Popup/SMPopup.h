// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "UI/Widget/SMActivatableWidget.h"

#include "SMPopup.generated.h"

class USMPopup;

DECLARE_DELEGATE_OneParam(FOnPopupSubmit, USMPopup*);

DECLARE_DELEGATE(FOnPopupClose);

/**
 * StereoMix Popup Widget
 */
UCLASS(Abstract)
class STEREOMIX_API USMPopup : public USMActivatableWidget
{
	GENERATED_BODY()

public:
	FOnPopupSubmit OnSubmit;
	FOnPopupClose OnClose;

	USMPopup();

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	virtual void NativeOnActivated() override;

	virtual bool NativeOnHandleBackAction() override;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCommonButtonBase> SubmitButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UCommonButtonBase> CloseButton;

	virtual bool CanSubmit() { return true; }

	virtual void PerformSubmit();

	virtual void OnSubmitButtonClicked();

	virtual void OnCloseButtonClicked();
};
