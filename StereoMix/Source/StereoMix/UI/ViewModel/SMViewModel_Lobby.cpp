// Copyright Surround, Inc. All Rights Reserved.


#include "SMViewModel_Lobby.h"

#include "Connection/SMClientConnectionSubsystem.h"
#include "Kismet/GameplayStatics.h"

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
	// SetStatus(FText::FromString(TEXT("방을 찾는 중 입니다...")));
}

void USMViewModel_Lobby::Back()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("L_MainMenu"));
}
