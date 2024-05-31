// Copyright Surround, Inc. All Rights Reserved.


#include "SMPopupViewModel.h"

void USMPopupViewModel::Close() const
{
	OnPopupClosed.Execute();
}
