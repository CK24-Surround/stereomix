// Copyright Surround, Inc. All Rights Reserved.


#include "SMValidationBlueprintLibrary.h"

bool USMValidationBlueprintLibrary::ValidateName(const FString& Name)
{
	static const FRegexPattern ValidUserNamePattern(TEXT("^[가-힣a-zA-Z0-9]{1,10}$"));

	FRegexMatcher UserNameMatcher(ValidUserNamePattern, Name);
	return UserNameMatcher.FindNext();
}

bool USMValidationBlueprintLibrary::ValidateRoomCode(const FString& RoomCode)
{
	static const FRegexPattern ValidUserNamePattern(TEXT("^[A-Z0-9]{6}$"));

	FRegexMatcher UserNameMatcher(ValidUserNamePattern, RoomCode);
	return UserNameMatcher.FindNext();
}
