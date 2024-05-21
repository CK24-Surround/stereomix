// Copyright Surround, Inc. All Rights Reserved.


#include "SMViewModel_GuestLogin.h"

#include "Connection/SMClientConnectionSubsystem.h"

void USMViewModel_GuestLogin::Submit()
{
	SetSubmitButtonEnabled(false);

	if (USMClientConnectionSubsystem* ClientConnection = GetWorld()->GetGameInstance()->GetSubsystem<USMClientConnectionSubsystem>())
	{
		ClientConnection->GuestLogin(GetUserName().ToString());
	}
}
