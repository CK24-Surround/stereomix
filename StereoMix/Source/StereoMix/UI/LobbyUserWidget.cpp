// Copyright Surround, Inc. All Rights Reserved.


#include "LobbyUserWidget.h"

#include "Components/Button.h"
#include "Player/SMLobbyController.h"

void ULobbyUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	check(CreateRoomButton)
	CreateRoomButton->OnClicked.AddUniqueDynamic(this, &ULobbyUserWidget::OnCreateRoomButtonClicked);

	check(JoinRoomButton)
	JoinRoomButton->OnClicked.AddUniqueDynamic(this, &ULobbyUserWidget::OnJoinRoomButtonClicked);
}

void ULobbyUserWidget::OnCreateRoomButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("OnCreateRoomButtonClicked"))
	ASMLobbyController* LobbyController = Cast<ASMLobbyController>(GetOwningPlayer());
	if (LobbyController)
	{
		UE_LOG(LogTemp, Log, TEXT("CreateRoom"))
		LobbyController->CreateRoom("TestRoom");
	}
}

void ULobbyUserWidget::OnJoinRoomButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("OnJoinRoomButtonClicked"))
	ASMLobbyController* LobbyController = Cast<ASMLobbyController>(GetOwningPlayer());
	if (LobbyController)
	{
		UE_LOG(LogTemp, Log, TEXT("JoinRoom"))
		LobbyController->JoinRoom("test-room-id");
	}
}

void ULobbyUserWidget::UpdateEnabled(bool bEnabled)
{
	CreateRoomButton->SetIsEnabled(bEnabled);
	JoinRoomButton->SetIsEnabled(bEnabled);
}
