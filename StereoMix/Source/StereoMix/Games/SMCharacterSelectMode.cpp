// Copyright Surround, Inc. All Rights Reserved.


#include "SMCharacterSelectMode.h"

#include "SMCharacterSelectState.h"
#include "Player/SMCharacterSelectPlayerState.h"
#include "Player/SMFrontendPlayerController.h"
#include "Session/SMGameSession.h"
#include "Session/SMRoomSession.h"

ASMCharacterSelectMode::ASMCharacterSelectMode()
{
	bUseSeamlessTravel = true;
	GameSessionClass = ASMGameSession::StaticClass();
	GameStateClass = ASMCharacterSelectState::StaticClass();
	PlayerStateClass =ASMCharacterSelectPlayerState::StaticClass();
	PlayerControllerClass = ASMFrontendPlayerController::StaticClass();
}
