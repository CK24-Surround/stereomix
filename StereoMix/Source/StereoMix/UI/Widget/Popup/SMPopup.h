// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "UI/Widget/SMActivatableWidget.h"
#include "SMPopup.generated.h"

/**
 * StereoMix Popup Widget
 */
UCLASS(Abstract)
class STEREOMIX_API USMPopup : public USMActivatableWidget
{
	GENERATED_BODY()

public:
	FSimpleDelegate OnSubmit;
	FSimpleDelegate OnClose;
	
	USMPopup();

	virtual void NativeConstruct() override;

	virtual void NativeOnActivated() override;
	
	virtual bool NativeOnHandleBackAction() override;

protected:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UCommonButtonBase> SubmitButton;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<UCommonButtonBase> CloseButton;
	
	virtual bool CanSubmit() { return true; }

	virtual void PerformSubmit();
	
	virtual void OnSubmitButtonClicked();

	virtual void OnCloseButtonClicked();
};
