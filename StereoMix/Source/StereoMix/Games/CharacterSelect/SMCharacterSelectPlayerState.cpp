// Copyright Surround, Inc. All Rights Reserved.


#include "SMCharacterSelectPlayerState.h"

#include "Net/UnrealNetwork.h"
#include "SMCharacterSelectState.h"


ASMCharacterSelectPlayerState::ASMCharacterSelectPlayerState()
{
	CurrentState = ECharacterSelectPlayerStateType::Loading;
	bCopyCharacterTypeOnSeamlessTravel = false;
}

void ASMCharacterSelectPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentState)
}

void ASMCharacterSelectPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (DefaultOptions)
	{
		SetTeam(DefaultOptions->DefaultTeam);
	}
}

void ASMCharacterSelectPlayerState::BeginPlay()
{
	Super::BeginPlay();
	CharacterSelectState = GetWorld()->GetGameState<ASMCharacterSelectState>();
}

void ASMCharacterSelectPlayerState::ChangeCharacterType_Implementation(ESMCharacterType NewCharacterType)
{
	if (SetCharacterType(NewCharacterType))
	{
		ResponseChangeCharacter(true, NewCharacterType);
	}
	else
	{
		ResponseChangeCharacter(false, ESMCharacterType::None);
	}
}

void ASMCharacterSelectPlayerState::ResponseChangeCharacter_Implementation(bool bSuccess, ESMCharacterType NewCharacterType)
{
	if (OnCharacterChangeResponse.IsBound())
	{
		OnCharacterChangeResponse.Broadcast(bSuccess, NewCharacterType);
	}
}

bool ASMCharacterSelectPlayerState::CanChangeCharacterType(const ESMCharacterType NewCharacterType) const
{
	return Super::CanChangeCharacterType(NewCharacterType) && CharacterSelectState->GetCurrentState() == ECharacterSelectionStateType::Select;
}

void ASMCharacterSelectPlayerState::OnCharacterTypeChanged(const ESMCharacterType PreviousCharacterType, const ESMCharacterType NewCharacterType)
{
	Super::OnCharacterTypeChanged(PreviousCharacterType, NewCharacterType);
}

void ASMCharacterSelectPlayerState::SetCurrentState_Implementation(const ECharacterSelectPlayerStateType NewState)
{
	CurrentState = NewState;
	if (OnCurrentStateChanged.IsBound())
	{
		OnCurrentStateChanged.Broadcast(CurrentState);
	}
}

void ASMCharacterSelectPlayerState::OnRep_CurrentState()
{
	if (OnCurrentStateChanged.IsBound())
	{
		OnCurrentStateChanged.Broadcast(CurrentState);
	}
}
