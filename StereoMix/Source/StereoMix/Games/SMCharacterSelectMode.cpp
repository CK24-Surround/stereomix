// Copyright Surround, Inc. All Rights Reserved.


#include "SMCharacterSelectMode.h"

#include "SMCharacterSelectState.h"
#include "Player/SMFrontendPlayerController.h"
#include "Session/SMRoomSession.h"

ASMCharacterSelectMode::ASMCharacterSelectMode()
{
	GameSessionClass = ASMRoomSession::StaticClass();
	GameStateClass = ASMCharacterSelectState::StaticClass();
	PlayerControllerClass = ASMFrontendPlayerController::StaticClass();
}
