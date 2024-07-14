// Copyright Surround, Inc. All Rights Reserved.


#include "SMPopup.h"

USMPopup::USMPopup()
{
	bAutoActivate = true;
	bIsBackHandler = true;
	bIsModal = true;
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

void USMPopup::NativeDestruct()
{
	Super::NativeDestruct();

	if (SubmitButton)
	{
		SubmitButton->OnClicked().RemoveAll(this);
	}
	if (CloseButton)
	{
		CloseButton->OnClicked().RemoveAll(this);
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
	(void)OnSubmit.ExecuteIfBound(this);
	DeactivateWidget();
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
