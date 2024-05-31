// Copyright Surround, Inc. All Rights Reserved.


#include "SMMainMenuViewModel.h"

#include "Backend/Client/SMClientAuthSubsystem.h"
#include "Kismet/GameplayStatics.h"

void USMMainMenuViewModel::InitializeViewModel(UWorld* InWorld)
{
	Super::InitializeViewModel(InWorld);

	if (!InWorld)
	{
		return;
	}

	USMClientAuthSubsystem* AuthSubsystem = InWorld->GetGameInstance()->GetSubsystem<USMClientAuthSubsystem>();
	if (AuthSubsystem->IsAuthenticated())
	{
		const USMUserAccount* UserAccount = AuthSubsystem->GetUserAccount();
		UserName = FText::FromString(UserAccount->GetUserName());
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(UserName);
	}
}

void USMMainMenuViewModel::StartGame()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("L_Lobby"));
}

void USMMainMenuViewModel::StartTutorial()
{
	// UGameplayStatics::OpenLevel(GetWorld(), TEXT("L_Tutorial"));
}

void USMMainMenuViewModel::QuitGame()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, true);
}
