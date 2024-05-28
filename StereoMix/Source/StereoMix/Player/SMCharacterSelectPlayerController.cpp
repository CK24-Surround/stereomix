// Copyright Surround, Inc. All Rights Reserved.


#include "SMCharacterSelectPlayerController.h"

#include "SMCharacterSelectPlayerState.h"
#include "Utilities/SMLog.h"

ASMCharacterSelectPlayerController::ASMCharacterSelectPlayerController()
{
}

void ASMCharacterSelectPlayerController::InitPlayerState()
{
	Super::InitPlayerState();

	CharacterSelectPlayerState = Cast<ASMCharacterSelectPlayerState>(PlayerState);
	if (CharacterSelectPlayerState.IsValid())
	{
		CharacterSelectPlayerState->CharacterTypeChangedEvent.AddDynamic(this, &ASMCharacterSelectPlayerController::OnCharacterTypeChanged);
	}
}

void ASMCharacterSelectPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ASMCharacterSelectPlayerController::OnCharacterTypeChanged(const ESMCharacterType NewCharacterType)
{
	NET_LOG(this, Verbose, TEXT("[SMCharacterSelectPlayerController] OnCharacterTypeChanged: %s"), *UEnum::GetValueAsString(NewCharacterType))
}
