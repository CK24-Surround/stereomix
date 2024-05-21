// Copyright Surround, Inc. All Rights Reserved.


#include "SMTitleMode.h"

#include "MVVMGameSubsystem.h"
#include "UI/ViewModel/SMViewModel_GuestLogin.h"

void ASMTitleMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	UMVVMGameSubsystem* MVVMGameSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UMVVMGameSubsystem>();
	if (MVVMGameSubsystem)
	{
		UMVVMViewModelCollectionObject* ViewModelCollection = MVVMGameSubsystem->GetViewModelCollection();
		if (ViewModelCollection)
		{
			USMViewModel_GuestLogin* GuestLoginVM = Cast<USMViewModel_GuestLogin>(ViewModelCollection->FindFirstViewModelInstanceOfType(USMViewModel_GuestLogin::StaticClass()));
			GuestLoginVM->SetUserName(FText::FromName(TEXT("Test1")));
		}
	}
}
