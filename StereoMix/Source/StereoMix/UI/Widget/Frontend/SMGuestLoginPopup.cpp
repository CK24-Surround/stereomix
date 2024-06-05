// Copyright Surround, Inc. All Rights Reserved.


#include "SMGuestLoginPopup.h"

USMGuestLoginPopup::USMGuestLoginPopup()
{
	bIsBackHandler = false;
}

void USMGuestLoginPopup::NativeConstruct()
{
	Super::NativeConstruct();

	UserNameInputBox->OnTextChanged.AddUniqueDynamic(this, &USMGuestLoginPopup::OnUserNameTextChanged);
}

void USMGuestLoginPopup::NativeOnActivated()
{
	Super::NativeOnActivated();
	UserNameInputBox->SetText(FText::GetEmpty());
	SubmitButton->SetIsEnabled(false);
}

bool USMGuestLoginPopup::CanSubmit()
{
	return IsValidUserName(GetUserName().ToString());
}

void USMGuestLoginPopup::PerformSubmit()
{
	Super::PerformSubmit();
}

void USMGuestLoginPopup::OnUserNameTextChanged(const FText& ChangedUserName)
{
	SubmitButton->SetIsEnabled(IsValidUserName(ChangedUserName.ToString()));
}

bool USMGuestLoginPopup::IsValidUserName(const FString& UserName) const
{
	static const FRegexPattern ValidUserNamePattern(TEXT("^[가-힣a-zA-Z0-9]*$"));

	if (UserName.Len() > MaxUserNameLength)
	{
		return false;
	}
	
	FRegexMatcher UserNameMatcher(ValidUserNamePattern, UserName);
	return UserNameMatcher.FindNext();
}
