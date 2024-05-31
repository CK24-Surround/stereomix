// Copyright Surround, Inc. All Rights Reserved.


#include "SMRoomCodeViewModel.h"

#include "MVVMGameSubsystem.h"
#include "MVVMSubsystem.h"
#include "Connection/SMClientConnectionSubsystem.h"

void USMRoomCodeViewModel::Submit()
{
	if (IsValidRoomShortCode(RoomShortCodeText) && OnSubmit.IsBound())
	{
		OnSubmit.Execute();
	}
}

void USMRoomCodeViewModel::Cancel()
{
	if (OnCancel.IsBound())
	{
		OnCancel.Execute();
	}
}

bool USMRoomCodeViewModel::IsValidRoomShortCode(const FText& RoomShortCodeToCheck)
{
	static const FRegexPattern ValidUserNamePattern(TEXT("^[A-Z0-9]{6}$"));

	FRegexMatcher UserNameMatcher(ValidUserNamePattern, RoomShortCodeToCheck.ToString());
	return UserNameMatcher.FindNext();
}
