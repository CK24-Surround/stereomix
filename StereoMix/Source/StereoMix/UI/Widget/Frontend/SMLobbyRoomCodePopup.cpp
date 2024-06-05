// Copyright Surround, Inc. All Rights Reserved.


#include "SMLobbyRoomCodePopup.h"

USMLobbyRoomCodePopup::USMLobbyRoomCodePopup()
{
	bIsBackHandler = true;
	bIsModal = true;
}

void USMLobbyRoomCodePopup::NativeConstruct()
{
	Super::NativeConstruct();

	RoomCodeInputBox->OnTextChanged.AddUniqueDynamic(this, &USMLobbyRoomCodePopup::OnRoomCodeTextChanged);
}

void USMLobbyRoomCodePopup::NativeOnActivated()
{
	Super::NativeOnActivated();
	RoomCodeInputBox->SetText(FText::GetEmpty());
	SubmitButton->SetIsEnabled(false);
}

bool USMLobbyRoomCodePopup::CanSubmit()
{
	return IsValidRoomCode(GetRoomCode().ToString());
}

void USMLobbyRoomCodePopup::PerformSubmit()
{
	Super::PerformSubmit();
}

void USMLobbyRoomCodePopup::OnRoomCodeTextChanged(const FText& ChangedRoomCode)
{
	// cut the string to 6 characters
	FString RoomCode = ChangedRoomCode.ToString().ToUpper();
	if (RoomCode.Len() > RoomCodeDesiredLength)
	{
		RoomCode = RoomCode.Left(RoomCodeDesiredLength);
	}
	RoomCodeInputBox->SetText(FText::FromString(RoomCode));
	SubmitButton->SetIsEnabled(IsValidRoomCode(RoomCode));
}

bool USMLobbyRoomCodePopup::IsValidRoomCode(const FString& RoomCode) const
{
	static const FRegexPattern ValidUserNamePattern(TEXT("^[A-Z0-9]*$"));

	if (RoomCode.Len() != RoomCodeDesiredLength)
	{
		return false;
	}

	FRegexMatcher UserNameMatcher(ValidUserNamePattern, RoomCode);
	return UserNameMatcher.FindNext();
}
