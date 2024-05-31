// Copyright Surround, Inc. All Rights Reserved.


#include "SMLobbyViewModel.h"

#include "Connection/SMClientConnectionSubsystem.h"
#include "Kismet/GameplayStatics.h"

void USMLobbyViewModel::Init()
{
	if ((ConnectionSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USMClientConnectionSubsystem>()))
	{
		ConnectionSubsystem->LobbyServiceStateChangedEvent.AddUniqueDynamic(this, &USMLobbyViewModel::OnLobbyServiceStateChanged);
		ConnectionSubsystem->CreateRoomEvent.AddUniqueDynamic(this, &USMLobbyViewModel::OnCreateRoom);
		ConnectionSubsystem->JoinRoomEvent.AddUniqueDynamic(this, &USMLobbyViewModel::OnJoinRoom);
		ConnectionSubsystem->ConnectLobbyService();
	}
}

void USMLobbyViewModel::Destroy()
{
	if (ConnectionSubsystem)
	{
		ConnectionSubsystem->LobbyServiceStateChangedEvent.RemoveDynamic(this, &USMLobbyViewModel::OnLobbyServiceStateChanged);
		ConnectionSubsystem->CreateRoomEvent.RemoveDynamic(this, &USMLobbyViewModel::OnCreateRoom);
		ConnectionSubsystem->JoinRoomEvent.RemoveDynamic(this, &USMLobbyViewModel::OnJoinRoom);
	}
}

void USMLobbyViewModel::CreateRoom()
{
	if (USMClientConnectionSubsystem* ClientConnection = GetWorld()->GetGameInstance()->GetSubsystem<USMClientConnectionSubsystem>())
	{
		SetAllButtonEnabled(false);
		ClientConnection->CreateRoom(TEXT("TestRoom"), EGrpcLobbyRoomVisibility::ROOM_VISIBILITY_PUBLIC, EGrpcLobbyGameMode::GAME_MODE_DEFAULT, EGrpcLobbyGameMap::GAME_MAP_DEFAULT);
	}
}

void USMLobbyViewModel::QuickMatch()
{
	// SetIsAllButtonEnabled(false);
}

void USMLobbyViewModel::JoinRoom()
{
	if (OnRequestJoinRoom.IsBound())
	{
		OnRequestJoinRoom.Execute();
	}
	// SetStatus(FText::FromString(TEXT("방을 찾는 중 입니다...")));
}

void USMLobbyViewModel::Back()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("L_MainMenu"));
}

void USMLobbyViewModel::OnLobbyServiceStateChanged(EGrpcServiceState ChangedState)
{
}

void USMLobbyViewModel::OnCreateRoom(EGrpcResultCode Result, EGrpcLobbyRoomDeploymentStatus DeployStatus, const FString& ConnectionUrl)
{
}

void USMLobbyViewModel::OnJoinRoom(EGrpcResultCode Result, const FString& ConnectionUrl)
{
}
