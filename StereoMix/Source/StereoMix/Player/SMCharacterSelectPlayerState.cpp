// Copyright Surround, Inc. All Rights Reserved.


#include "SMCharacterSelectPlayerState.h"

#include "Utilities/SMLog.h"

ASMCharacterSelectPlayerState::ASMCharacterSelectPlayerState()
{
}

void ASMCharacterSelectPlayerState::BeginPlay()
{
	Super::BeginPlay();
	NET_LOG(this, Verbose, TEXT("ASMCharacterSelectPlayerState::BeginPlay"))
}
