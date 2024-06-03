// Copyright Surround, Inc. All Rights Reserved.


#include "SMCharacterSelectMode.h"

#include "CountdownTimerComponent.h"
#include "SMCharacterSelectState.h"
#include "StereoMix.h"
#include "Player/SMCharacterSelectPlayerController.h"
#include "Player/SMCharacterSelectPlayerState.h"
#include "Session/SMGameSession.h"

ASMCharacterSelectMode::ASMCharacterSelectMode()
{
	bUseSeamlessTravel = true;
	GameSessionClass = ASMGameSession::StaticClass();
	GameStateClass = ASMCharacterSelectState::StaticClass();
	PlayerStateClass = ASMCharacterSelectPlayerState::StaticClass();
	PlayerControllerClass = ASMCharacterSelectPlayerController::StaticClass();
}

void ASMCharacterSelectMode::InitGameState()
{
	Super::InitGameState();
	CharacterSelectState = CastChecked<ASMCharacterSelectState>(GameState);

	CharacterSelectState->GetCountdownTimer()->OnCountdownTick.AddDynamic(this, &ASMCharacterSelectMode::OnCountdownTick);
	CharacterSelectState->GetCountdownTimer()->OnCountdownFinished.AddDynamic(this, &ASMCharacterSelectMode::OnCountdownFinished);
}

void ASMCharacterSelectMode::StartPlay()
{
	Super::StartPlay();
	if (CharacterSelectState.IsValid())
	{
		CharacterSelectState->GetCountdownTimer()->StartCountdown(CharacterSelectState->CountdownTime);
	}
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void ASMCharacterSelectMode::OnCountdownTick()
{
}

void ASMCharacterSelectMode::OnCountdownFinished()
{
	if (CharacterSelectState.IsValid())
	{
		CharacterSelectState->SetCharacterSelectionState(ECharacterSelectionState::End);
		
		// 임시 코드
		for (TObjectPtr<APlayerState> PlayerState : CharacterSelectState->PlayerArray)
		{
			if (ASMCharacterSelectPlayerState* CharacterSelectPlayerState = Cast<ASMCharacterSelectPlayerState>(PlayerState))
			{
				if (CharacterSelectPlayerState->GetCharacterType() == ESMCharacterType::None)
				{
					// 임시로 기본 기타로 설정
					CharacterSelectPlayerState->SetCharacterType(ESMCharacterType::ElectricGuitar);
				}
				UE_LOG(LogStereoMix, Log, TEXT("Player %s selected %s."), *CharacterSelectPlayerState->GetPlayerName(), *UEnum::GetValueAsString(CharacterSelectPlayerState->GetCharacterType()))
			}
		}
		
		CharacterSelectState->GetCountdownTimer()->OnCountdownFinished.RemoveDynamic(this, &ASMCharacterSelectMode::OnCountdownFinished);
		CharacterSelectState->GetCountdownTimer()->OnCountdownFinished.AddDynamic(this, &ASMCharacterSelectMode::StartGame);

		UE_LOG(LogStereoMix, Verbose, TEXT("Game will start in 5 seconds."))
		CharacterSelectState->GetCountdownTimer()->StartCountdown(5);
	}
}

void ASMCharacterSelectMode::StartGame()
{
	if (GetWorld())
	{
		ProcessServerTravel("/Game/StereoMix/Levels/Round1/L_Round1");
	}
}
