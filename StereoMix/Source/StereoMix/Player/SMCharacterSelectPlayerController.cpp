// Copyright Surround, Inc. All Rights Reserved.


#include "SMCharacterSelectPlayerController.h"

#include "SMCharacterSelectPlayerState.h"
#include "Utilities/SMLog.h"

ASMCharacterSelectPlayerController::ASMCharacterSelectPlayerController()
{
}

void ASMCharacterSelectPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
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

void ASMCharacterSelectPlayerController::ChangeCharacterType_Implementation(ESMCharacterType NewCharacterType)
{
#if WITH_SERVER_CODE
	if (CharacterSelectPlayerState.IsValid())
	{
		CharacterSelectPlayerState->SetCharacterType(NewCharacterType);
	}
#endif
}

void ASMCharacterSelectPlayerController::OnCharacterTypeChanged(const ESMCharacterType NewCharacterType)
{
	NET_LOG(this, Verbose, TEXT("[SMCharacterSelectPlayerController] OnCharacterTypeChanged: %s"), *UEnum::GetValueAsString(NewCharacterType))
	CharacterTypeChangedEvent.Broadcast(NewCharacterType);
}
