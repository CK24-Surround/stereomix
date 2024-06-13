// Copyright Surround, Inc. All Rights Reserved.


#include "SMRoomCodeCopyButton.h"

#include "FunctionLibraries/SMPlatformBlueprintLibrary.h"

void USMRoomCodeCopyButton::SetRoomCode(const FString& RoomCode)
{
	RoomCodeTextBlock->SetText(FText::FromString(RoomCode));
}

void USMRoomCodeCopyButton::NativeOnClicked()
{
	Super::NativeOnClicked();

	USMPlatformBlueprintLibrary::CopyToClipboard(RoomCodeTextBlock->GetText().ToString());
}
