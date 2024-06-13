// Copyright Surround, Inc. All Rights Reserved.


#include "SMAuthSubsystem.h"

USMAuthSubsystem::USMAuthSubsystem() : Super(TEXT("AuthService")) {}

bool USMAuthSubsystem::IsAuthenticated() const
{
	return false;
}

void USMAuthSubsystem::RestAuthentication() {}
