// Copyright Surround, Inc. All Rights Reserved.


#include "SMViewModel_Popup.h"

void USMViewModel_Popup::Close() const
{
	OnPopupClosed.Execute();
}
