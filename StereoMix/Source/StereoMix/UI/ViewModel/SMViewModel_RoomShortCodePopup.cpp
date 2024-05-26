// Copyright Surround, Inc. All Rights Reserved.


#include "SMViewModel_RoomShortCodePopup.h"

#include "MVVMGameSubsystem.h"
#include "MVVMSubsystem.h"
#include "Connection/SMClientConnectionSubsystem.h"

void USMViewModel_RoomShortCodePopup::Submit()
{
	if (IsValidRoomShortCode(RoomShortCodeText) && OnSubmit.IsBound())
	{
		OnSubmit.Execute();
	}
}

void USMViewModel_RoomShortCodePopup::Cancel()
{
	if (OnCancel.IsBound())
	{
		OnCancel.Execute();
	}
}

bool USMViewModel_RoomShortCodePopup::IsValidRoomShortCode(const FText& RoomShortCodeToCheck)
{
	static const FRegexPattern ValidUserNamePattern(TEXT("^[A-Z0-9]{6}$"));

	FRegexMatcher UserNameMatcher(ValidUserNamePattern, RoomShortCodeToCheck.ToString());
	return UserNameMatcher.FindNext();
}
