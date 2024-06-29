// Copyright Surround, Inc. All Rights Reserved.


#include "SMLobbyProcessWidget.h"

#include "Backend/Client/SMClientLobbySubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "SMLobbyProgressSectionWidget.h"
#include "UI/Widget/Frontend/SMFrontendWidget.h"

USMLobbyProcessWidget::USMLobbyProcessWidget()
{
	bIsBackHandler = true;
}

void USMLobbyProcessWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void USMLobbyProcessWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	AuthSubsystem = GetGameInstance()->GetSubsystem<USMClientAuthSubsystem>();
	LobbySubsystem = GetGameInstance()->GetSubsystem<USMClientLobbySubsystem>();

	UiState = ELobbyProcessUiState::Idle;
	SetProgressVisibility(ESlateVisibility::Collapsed);
}

bool USMLobbyProcessWidget::NativeOnHandleBackAction()
{
	if (UiState == ELobbyProcessUiState::Success)
	{
		return false;
	}

	if (Super::NativeOnHandleBackAction())
	{
		if (LobbySubsystem)
		{
			LobbySubsystem->Cancel();
		}
		return true;
	}
	return false;
}

void USMLobbyProcessWidget::SetProgressText(const FText& Text) const
{
	if (Progress)
	{
		Progress->SetProgressText(Text);
	}
}

void USMLobbyProcessWidget::SetProgressVisibility(const ESlateVisibility NewVisibility) const
{
	if (Progress)
	{
		Progress->SetVisibility(NewVisibility);
	}
}

bool USMLobbyProcessWidget::CheckIsAuthenticated() const
{
	if (AuthSubsystem && AuthSubsystem->IsAuthenticated())
	{
		return true;
	}

	GetParentFrontendWidget()->ShowAlert(TEXT("인증 정보가 유효하지 않습니다."))->OnSubmit.BindWeakLambda(this, [&] { UGameplayStatics::OpenLevelBySoftObjectPtr(this, GetWorld()); });
	return false;
}

void USMLobbyProcessWidget::ConnectToGameServer(const FString& ServerUrl) const
{
	if (!AuthSubsystem)
	{
		return;
	}

	const USMUserAccount* UserAccount = AuthSubsystem->GetUserAccount();
	const FString& UserId = UserAccount->GetUserId();
	const FString& UserName = UserAccount->GetUserName();

	UGameplayStatics::OpenLevel(GetWorld(), FName(ServerUrl), true, FString::Printf(TEXT("UserId=%s?Nickname=%s"), *UserId, *UserName));

	// GetOwningPlayer()->ClientTravel(FString::Printf(TEXT("%s?UserId=%s?Nickname=%s"), *ServerUrl, *UserId, *UserName), TRAVEL_Absolute);
}
