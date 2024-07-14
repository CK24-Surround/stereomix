// Copyright Surround, Inc. All Rights Reserved.


#include "SMGuestLoginViewModel.h"

#include "MVVMSubsystem.h"
#include "StereoMixLog.h"

void USMGuestLoginViewModel::SetInputUserName(const FText& UserName)
{
	UE_MVVM_SET_PROPERTY_VALUE(InputUserName, UserName);
	SetUiState(IsValidUserName(UserName) ? ELoginViewModelUiState::ReadyToSubmit : ELoginViewModelUiState::Idle);
}

void USMGuestLoginViewModel::OnSubmit()
{
	Super::OnSubmit();

	if (!AuthSubsystem.IsValid())
	{
		UE_LOG(LogStereoMix, Warning, TEXT("AuthSubsystem is not valid"));
		return;
	}

	AuthSubsystem->LoginAsGuest(GetUserName().ToString());
	SetUiState(ELoginViewModelUiState::Requesting);
	// TODO: 로그인 중 알림
}

bool USMGuestLoginViewModel::IsValidUserName(const FText& UserNameToCheck)
{
	static const FRegexPattern ValidUserNamePattern(TEXT("^[가-힣a-zA-Z0-9]{1,20}$"));

	FRegexMatcher UserNameMatcher(ValidUserNamePattern, UserNameToCheck.ToString());
	return UserNameMatcher.FindNext();
}
