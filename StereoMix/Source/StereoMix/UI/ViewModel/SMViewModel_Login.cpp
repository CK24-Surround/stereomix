// Copyright Surround, Inc. All Rights Reserved.


#include "SMViewModel_Login.h"

USMViewModel_Login::USMViewModel_Login()
{
	bSubmitButtonEnabled = false;
}

void USMViewModel_Login::Submit()
{
	if (OnSubmit.IsBound())
	{
		SetSubmitButtonEnabled(false);
		OnSubmit.Execute();
	}
}
