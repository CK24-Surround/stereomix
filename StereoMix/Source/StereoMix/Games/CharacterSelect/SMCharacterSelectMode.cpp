// Copyright Surround, Inc. All Rights Reserved.


#include "SMCharacterSelectMode.h"

#include "Controllers/SMCharacterSelectPlayerController.h"
#include "SMCharacterSelectPlayerState.h"
#include "SMCharacterSelectState.h"
#include "Session/SMGameSession.h"
#include "StereoMixLog.h"
#include "GameInstance/SMGameInstance.h"
#include "Games/SMCountdownTimerComponent.h"

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
}

void ASMCharacterSelectMode::StartPlay()
{
	Super::StartPlay();

	RemainingWaitingTime = 30;
	GetWorldTimerManager().SetTimer(WaitingTimerHandle, this, &ASMCharacterSelectMode::WaitingTimerTick, 1.0f, true, 10.0f);

	CharacterSelectState->GetCountdownTimer()->OnCountdownTick.AddDynamic(this, &ASMCharacterSelectMode::OnCountdownTick);
}

void ASMCharacterSelectMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void ASMCharacterSelectMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}

bool ASMCharacterSelectMode::IsAllPlayerReadyToSelect() const
{
	if (CharacterSelectState.IsValid())
	{
		for (TObjectPtr<APlayerState> PlayerState : CharacterSelectState->PlayerArray)
		{
			const ASMCharacterSelectPlayerState* CharacterSelectPlayerState = Cast<ASMCharacterSelectPlayerState>(PlayerState);
			if (CharacterSelectPlayerState && CharacterSelectPlayerState->GetCurrentState() == ECharacterSelectPlayerStateType::Loading)
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

void ASMCharacterSelectMode::ImmediateStart()
{
	if (!CharacterSelectState.IsValid() || CharacterSelectState->GetCurrentState() != ECharacterSelectionStateType::Select)
	{
		return;
	}

	CharacterSelectState->GetCountdownTimer()->CancelCountdown();
	OnCharacterSelectCountdownFinished();
}

void ASMCharacterSelectMode::WaitingTimerTick()
{
	RemainingWaitingTime--;

	if (IsAllPlayerReadyToSelect() || RemainingWaitingTime <= 0)
	{
		GetWorldTimerManager().ClearTimer(WaitingTimerHandle);
		UE_LOG(LogStereoMix, Verbose, TEXT("Character select countdown started."))
		CharacterSelectState->SetCurrentState(ECharacterSelectionStateType::Select);

		CharacterSelectState->GetCountdownTimer()->OnCountdownFinished.AddUniqueDynamic(this, &ASMCharacterSelectMode::OnCharacterSelectCountdownFinished);
		CharacterSelectState->GetCountdownTimer()->StartCountdown(CharacterSelectState->CharacterSelectCountdownTime);
		return;
	}

	UE_LOG(LogStereoMix, Verbose, TEXT("Waiting for players to join. %d seconds remaining."), RemainingWaitingTime)
}

void ASMCharacterSelectMode::OnCountdownTick()
{
	// 1초마다 플레이어들이 선택한 캐릭터가 중복되지 않도록 체크
	if (CharacterSelectState.IsValid())
	{
		TArray EDMAvailableCharacterTypes = { ESMCharacterType::ElectricGuitar, ESMCharacterType::Piano, ESMCharacterType::Bass };
		TArray FBAvailableCharacterTypes = { ESMCharacterType::ElectricGuitar, ESMCharacterType::Piano, ESMCharacterType::Bass };
		for (TObjectPtr<APlayerState> PlayerState : CharacterSelectState->PlayerArray)
		{
			if (ASMCharacterSelectPlayerState* CharacterSelectPlayerState = Cast<ASMCharacterSelectPlayerState>(PlayerState))
			{
				if (CharacterSelectPlayerState->GetCharacterType() == ESMCharacterType::None)
				{
					continue;
				}

				if (CharacterSelectPlayerState->GetTeam() == ESMTeam::EDM)
				{
					if (!EDMAvailableCharacterTypes.Contains(CharacterSelectPlayerState->GetCharacterType()))
					{
						CharacterSelectPlayerState->ChangeCharacterType(ESMCharacterType::None);
					}
					EDMAvailableCharacterTypes.Remove(CharacterSelectPlayerState->GetCharacterType());
					continue;
				}

				if (CharacterSelectPlayerState->GetTeam() == ESMTeam::FutureBass)
				{
					if (!FBAvailableCharacterTypes.Contains(CharacterSelectPlayerState->GetCharacterType()))
					{
						CharacterSelectPlayerState->ChangeCharacterType(ESMCharacterType::None);
					}
					FBAvailableCharacterTypes.Remove(CharacterSelectPlayerState->GetCharacterType());
				}
			}
		}
	}
}

void ASMCharacterSelectMode::OnCharacterSelectCountdownFinished()
{
	if (CharacterSelectState.IsValid())
	{
		auto InitializeCharacterTypeMap = [] {
			TMap<ESMCharacterType, TArray<ASMCharacterSelectPlayerState*>> CharacterTypesMap;
			for (const ESMCharacterType& Type : { ESMCharacterType::ElectricGuitar, ESMCharacterType::Piano, ESMCharacterType::Bass })
			{
				CharacterTypesMap.Add(Type, TArray<ASMCharacterSelectPlayerState*>());
			}
			return CharacterTypesMap;
		};

		TMap<ESMCharacterType, TArray<ASMCharacterSelectPlayerState*>> EDMCharacterTypesMap = InitializeCharacterTypeMap();
		TMap<ESMCharacterType, TArray<ASMCharacterSelectPlayerState*>> FBCharacterTypesMap = InitializeCharacterTypeMap();

		TArray EDMAvailableCharacterTypes = { ESMCharacterType::ElectricGuitar, ESMCharacterType::Piano, ESMCharacterType::Bass };
		TArray FBAvailableCharacterTypes = { ESMCharacterType::ElectricGuitar, ESMCharacterType::Piano, ESMCharacterType::Bass };

		for (TObjectPtr<APlayerState> PlayerState : CharacterSelectState->PlayerArray)
		{
			if (ASMCharacterSelectPlayerState* CharacterSelectPlayerState = Cast<ASMCharacterSelectPlayerState>(PlayerState))
			{
				if (CharacterSelectPlayerState->GetCharacterType() != ESMCharacterType::None)
				{
					continue;
				}

				auto UpdateMapAndAvailableTypes = [&](TMap<ESMCharacterType, TArray<ASMCharacterSelectPlayerState*>>& CharacterMap, TArray<ESMCharacterType>& AvailableTypes) {
					if (CharacterMap.Contains(CharacterSelectPlayerState->GetCharacterType()))
					{
						AvailableTypes.Remove(CharacterSelectPlayerState->GetCharacterType());
						CharacterMap[CharacterSelectPlayerState->GetCharacterType()].AddUnique(CharacterSelectPlayerState);
					}
				};

				UpdateMapAndAvailableTypes(EDMCharacterTypesMap, EDMAvailableCharacterTypes);
				UpdateMapAndAvailableTypes(FBCharacterTypesMap, FBAvailableCharacterTypes);
			}
		}

		// 중복된 캐릭터 타입 초기화
		auto ResetDuplicatedTypes = [](TMap<ESMCharacterType, TArray<ASMCharacterSelectPlayerState*>>& CharacterMap, TArray<ESMCharacterType>& AvailableTypes) {
			for (const auto& CharacterTypeMap : CharacterMap)
			{
				if (CharacterTypeMap.Value.Num() > 1)
				{
					AvailableTypes.AddUnique(CharacterTypeMap.Key);
					for (ASMCharacterSelectPlayerState* PlayerState : CharacterTypeMap.Value)
					{
						PlayerState->ChangeCharacterType(ESMCharacterType::None);
					}
				}
			}
		};

		ResetDuplicatedTypes(EDMCharacterTypesMap, EDMAvailableCharacterTypes);
		ResetDuplicatedTypes(FBCharacterTypesMap, FBAvailableCharacterTypes);

		// 아직 선택하지 않은 플레이어들을 무작위 캐릭터로 설정
		for (TObjectPtr<APlayerState> PlayerState : CharacterSelectState->PlayerArray)
		{
			if (ASMCharacterSelectPlayerState* CharacterSelectPlayerState = Cast<ASMCharacterSelectPlayerState>(PlayerState))
			{
				if (CharacterSelectPlayerState->GetCharacterType() != ESMCharacterType::None)
				{
					continue;
				}

				auto AssignRandomCharacter = [&](TArray<ESMCharacterType>& AvailableTypes) {
					if (AvailableTypes.Num() == 0)
					{
						CharacterSelectPlayerState->ChangeCharacterType(ESMCharacterType::ElectricGuitar);
					}
					else
					{
						const ESMCharacterType RandomCharacterType = AvailableTypes[FMath::RandRange(0, AvailableTypes.Num() - 1)];
						CharacterSelectPlayerState->ChangeCharacterType(RandomCharacterType);
						AvailableTypes.Remove(RandomCharacterType);
					}

					UE_LOG(LogStereoMix, Log, TEXT("Player %s automatically selected character %s."),
						*CharacterSelectPlayerState->GetPlayerName(),
						*UEnum::GetValueAsString(CharacterSelectPlayerState->GetCharacterType()));
				};

				if (CharacterSelectPlayerState->GetTeam() == ESMTeam::EDM)
				{
					AssignRandomCharacter(EDMAvailableCharacterTypes);
				}
				else if (CharacterSelectPlayerState->GetTeam() == ESMTeam::FutureBass)
				{
					AssignRandomCharacter(FBAvailableCharacterTypes);
				}
			}
		}

		// CharacterSelectState->GetCountdownTimer()->OnCountdownFinished.RemoveDynamic(this, &ASMCharacterSelectMode::OnCountdownFinished);
		CharacterSelectState->GetCountdownTimer()->OnCountdownFinished.AddUniqueDynamic(this, &ASMCharacterSelectMode::StartGame);

		UE_LOG(LogStereoMix, Verbose, TEXT("Game will start in 5 seconds."))
		for (auto TargetPlayerState : GameState->PlayerArray)
		{
			ASMPlayerController* TargetPlayerController = Cast<ASMPlayerController>(TargetPlayerState->GetOwner());
			TargetPlayerController->ClientReceiveChat(TEXT("시스템"), FString::Printf(TEXT("5초 뒤에 게임이 시작됩니다.")));
		}

		CharacterSelectState->SetCurrentState(ECharacterSelectionStateType::End);
		CharacterSelectState->GetCountdownTimer()->StartCountdown(5);
	}
}

void ASMCharacterSelectMode::StartGame()
{
	const UWorld* World = GetWorld();
	const USMGameInstance* GameInstance = World ? World->GetGameInstance<USMGameInstance>() : nullptr;
	const TArray<FSoftObjectPath>& StageLevels = GameInstance ? GameInstance->GetStageLevels() : TArray<FSoftObjectPath>();
	if (StageLevels.IsValidIndex(0))
	{
		ProcessServerTravel(StageLevels[0].GetLongPackageName());
	}
}
