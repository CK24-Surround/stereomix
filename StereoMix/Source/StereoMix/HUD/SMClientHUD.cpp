// Copyright Surround, Inc. All Rights Reserved.


#include "SMClientHUD.h"

#include "GameFramework/PlayerState.h"

void ASMClientHUD::InitPlayerState(APlayerState* PlayerState)
{
	OwningPlayerState = PlayerState;
}
