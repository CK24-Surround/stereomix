// Copyright Surround, Inc. All Rights Reserved.


#include "SMViewModel_CharacterSelect.h"

#include "Games/CountdownTimerComponent.h"
#include "Games/SMCharacterSelectState.h"
#include "Player/SMCharacterSelectPlayerState.h"

void USMViewModel_CharacterSelect::BindGameState(ASMCharacterSelectState* GameState)
{
	if (GameState)
	{
		OwningGameState = GameState;
		UE_MVVM_SET_PROPERTY_VALUE(TimerTime, OwningGameState->CountdownTime);
		UE_MVVM_SET_PROPERTY_VALUE(RemainingTime, OwningGameState->CountdownTime);

		if (const TObjectPtr<APlayerState>& PlayerState = GameState->GetWorld()->GetFirstPlayerController()->PlayerState)
		{
			if (ASMCharacterSelectPlayerState* CharacterSelectPlayerState = Cast<ASMCharacterSelectPlayerState>(PlayerState))
			{
				LocalPlayerState = CharacterSelectPlayerState;
				UE_MVVM_SET_PROPERTY_VALUE(LocalPlayerTeam, LocalPlayerState->GetTeam());
			}
		}

		OwningGameState->GetCountdownTimer()->OnCountdownTick.AddDynamic(this, &USMViewModel_CharacterSelect::OnCountdownTick);
	}
}

void USMViewModel_CharacterSelect::OnCountdownTick()
{
	if (OwningGameState.IsValid())
	{
		UE_MVVM_SET_PROPERTY_VALUE(RemainingTime, OwningGameState->GetCountdownTimer()->GetRemainingTime());
	}
}
