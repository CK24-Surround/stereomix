// Copyright Surround, Inc. All Rights Reserved.


#include "SMViewModel_Lobby.h"

#include "Connection/SMClientConnectionSubsystem.h"
#include "Kismet/GameplayStatics.h"

void USMViewModel_Lobby::Init()
{
	if ((ConnectionSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USMClientConnectionSubsystem>()))
	{
		ConnectionSubsystem->LobbyServiceStateChangedEvent.AddUniqueDynamic(this, &USMViewModel_Lobby::OnLobbyServiceStateChanged);
		ConnectionSubsystem->CreateRoomEvent.AddUniqueDynamic(this, &USMViewModel_Lobby::OnCreateRoom);
		ConnectionSubsystem->JoinRoomEvent.AddUniqueDynamic(this, &USMViewModel_Lobby::OnJoinRoom);
		ConnectionSubsystem->ConnectLobbyService();
	}
}

void USMViewModel_Lobby::Destroy()
{
	if (ConnectionSubsystem)
	{
		ConnectionSubsystem->LobbyServiceStateChangedEvent.RemoveDynamic(this, &USMViewModel_Lobby::OnLobbyServiceStateChanged);
		ConnectionSubsystem->CreateRoomEvent.RemoveDynamic(this, &USMViewModel_Lobby::OnCreateRoom);
		ConnectionSubsystem->JoinRoomEvent.RemoveDynamic(this, &USMViewModel_Lobby::OnJoinRoom);
	}
}

void USMViewModel_Lobby::CreateRoom()
{
	if (USMClientConnectionSubsystem* ClientConnection = GetWorld()->GetGameInstance()->GetSubsystem<USMClientConnectionSubsystem>())
	{
		ClientConnection->CreateRoom(TEXT("TestRoom"), EGrpcLobbyRoomVisibility::ROOM_VISIBILITY_PUBLIC, EGrpcLobbyGameMode::GAME_MODE_DEFAULT, EGrpcLobbyGameMap::GAME_MAP_DEFAULT);
	}
}

void USMViewModel_Lobby::QuickMatch()
{
	// SetIsAllButtonEnabled(false);
}

void USMViewModel_Lobby::JoinRoom()
{
	if (OnRequestJoinRoom.IsBound())
	{
		OnRequestJoinRoom.Execute();
	}
	// SetStatus(FText::FromString(TEXT("방을 찾는 중 입니다...")));
}

void USMViewModel_Lobby::Back()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("L_MainMenu"));
}

void USMViewModel_Lobby::OnLobbyServiceStateChanged(EGrpcServiceState ChangedState)
{
}

void USMViewModel_Lobby::OnCreateRoom(EGrpcResultCode Result, EGrpcLobbyRoomDeploymentStatus DeployStatus, const FString& ConnectionUrl)
{
}

void USMViewModel_Lobby::OnJoinRoom(EGrpcResultCode Result, const FString& ConnectionUrl)
{
}
