// Copyright Surround, Inc. All Rights Reserved.


#include "SMRoomMode.h"

#include "SMRoomPlayerState.h"
#include "SMRoomSession.h"
#include "SMRoomState.h"

ASMRoomMode::ASMRoomMode()
{
	bUseSeamlessTravel = true;

	GameSessionClass = ASMRoomSession::StaticClass();
	GameStateClass = ASMRoomState::StaticClass();
	PlayerStateClass = ASMRoomPlayerState::StaticClass();
}

void ASMRoomMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
}
