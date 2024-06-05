// Copyright Surround, Inc. All Rights Reserved.


#include "SMPopup.h"

USMPopup::USMPopup()
{
	bAutoActivate = false;
	bIsBackHandler = true;
	bSupportsActivationFocus = true;
	bIsModal = true;
	SetIsFocusable(true);
}

void USMPopup::NativeConstruct()
{
	Super::NativeConstruct();

	if (SubmitButton)
	{
		SubmitButton->OnClicked().AddUObject(this, &USMPopup::OnSubmitButtonClicked);
	}
	if (CloseButton)
	{
		CloseButton->OnClicked().AddUObject(this, &USMPopup::OnCloseButtonClicked);
	}
}

void USMPopup::NativeOnActivated()
{
	Super::NativeOnActivated();
}

bool USMPopup::NativeOnHandleBackAction()
{
	if (Super::NativeOnHandleBackAction())
	{
		(void)OnClose.ExecuteIfBound();
		return true;
	}
	return false;
}

void USMPopup::PerformSubmit()
{
	DeactivateWidget();
	(void)OnSubmit.ExecuteIfBound();
}

void USMPopup::OnSubmitButtonClicked()
{
	if (CanSubmit())
	{
		PerformSubmit();
	}
}

void USMPopup::OnCloseButtonClicked()
{
	HandleBackAction();
}
