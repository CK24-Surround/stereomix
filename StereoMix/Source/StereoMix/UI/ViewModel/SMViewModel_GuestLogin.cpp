// Copyright Surround, Inc. All Rights Reserved.


#include "SMViewModel_GuestLogin.h"

#include "MVVMSubsystem.h"
#include "StereoMix.h"
#include "Connection/SMClientConnectionSubsystem.h"

bool USMViewModel_GuestLogin::IsValidUserName(const FText& UserNameToCheck)
{
	static const FRegexPattern ValidUserNamePattern(TEXT("^[가-힣a-zA-Z0-9]{1,20}$"));

	FRegexMatcher UserNameMatcher(ValidUserNamePattern, UserNameToCheck.ToString());
	return UserNameMatcher.FindNext();
}

void USMViewModel_GuestLogin::Submit()
{
	UE_LOG(LogStereoMix, Verbose, TEXT("Submit"))
	if (!IsValidUserName(GetUserName()))
	{
		return;
	}

	Super::Submit();
}
