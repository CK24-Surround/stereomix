// Copyright Surround, Inc. All Rights Reserved.


#include "SMAlertPopup.h"

USMAlertPopup::USMAlertPopup()
{
	bIsBackHandler = false;
}

void USMAlertPopup::SetAlertText(const FText& InAlertText)
{
	AlertTextBlock->SetText(InAlertText);
}

void USMAlertPopup::PerformSubmit()
{
	Super::PerformSubmit();
}
