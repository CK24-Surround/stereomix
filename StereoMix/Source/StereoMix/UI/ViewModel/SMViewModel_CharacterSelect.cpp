// Copyright Surround, Inc. All Rights Reserved.


#include "SMViewModel_CharacterSelect.h"

#include "StereoMix.h"
#include "Games/CountdownTimerComponent.h"
#include "Games/SMCharacterSelectState.h"
#include "Player/SMCharacterSelectPlayerState.h"

void USMViewModel_CharacterSelect::InitializeViewModel(UWorld* InWorld)
{
	Super::InitializeViewModel(InWorld);

	if (!InWorld)
	{
		return;
	}

	OwningGameState = Cast<ASMCharacterSelectState>(InWorld->GetGameState());
	if (OwningGameState.IsValid())
	{
		UE_LOG(LogStereoMix, Verbose, TEXT("[CharacterSelectViewModel] Current players: %d"), OwningGameState->PlayerArray.Num());
		UE_MVVM_SET_PROPERTY_VALUE(TimerTime, OwningGameState->CountdownTime);
		UE_MVVM_SET_PROPERTY_VALUE(RemainingTime, OwningGameState->CountdownTime);

		if (const TObjectPtr<APlayerState>& PlayerState = InWorld->GetFirstPlayerController()->PlayerState)
		{
			if (ASMCharacterSelectPlayerState* CharacterSelectPlayerState = Cast<ASMCharacterSelectPlayerState>(PlayerState))
			{
				LocalPlayerState = CharacterSelectPlayerState;
				auto Player1 = NewObject<USMViewModel_CharacterSelect_Player>(this);
				Player1->InitializeViewModel(GetWorld());
				PlayerEntries.Add(Player1);
				UE_MVVM_SET_PROPERTY_VALUE(LocalPlayerTeam, LocalPlayerState->GetTeam());
			}
			UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(PlayerEntries);
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
