// Copyright Surround, Inc. All Rights Reserved.


#include "SMCharacterSelectMode.h"

#include "Controllers/SMCharacterSelectPlayerController.h"
#include "SMCharacterSelectPlayerState.h"
#include "SMCharacterSelectState.h"
#include "Session/SMGameSession.h"
#include "StereoMixLog.h"
#include "GameInstance/SMGameInstance.h"
#include "Games/SMCountdownTimerComponent.h"
#include "Utilities/SMLog.h"

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

		TArray<TObjectPtr<APlayerState>> PlayerArray = CharacterSelectState->PlayerArray;
		PlayerArray.Sort([](const APlayerState& A, const APlayerState& B) {
			return A.GetActorGuid() < B.GetActorGuid();
		});

		for (TObjectPtr<APlayerState> PlayerState : PlayerArray)
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

		TArray<ASMCharacterSelectPlayerState*> EDMDuplicatedPlayers;
		TArray<ASMCharacterSelectPlayerState*> FBDuplicatedPlayers;

		// 중복된 플레이어 찾기
		for (TObjectPtr<APlayerState> PlayerState : CharacterSelectState->PlayerArray)
		{
			if (ASMCharacterSelectPlayerState* CharacterSelectPlayerState = Cast<ASMCharacterSelectPlayerState>(PlayerState))
			{
				if (CharacterSelectPlayerState->GetCharacterType() == ESMCharacterType::None)
				{
					(CharacterSelectPlayerState->GetTeam() == ESMTeam::EDM
						 ? EDMDuplicatedPlayers
						 : FBDuplicatedPlayers).Add(CharacterSelectPlayerState);
					continue;
				}

				TMap<ESMCharacterType, TArray<ASMCharacterSelectPlayerState*>>& TargetMap = (CharacterSelectPlayerState->GetTeam() == ESMTeam::EDM) ? EDMCharacterTypesMap : FBCharacterTypesMap;
				TArray<ESMCharacterType>& AvailableTypes = (CharacterSelectPlayerState->GetTeam() == ESMTeam::EDM) ? EDMAvailableCharacterTypes : FBAvailableCharacterTypes;

				TargetMap[CharacterSelectPlayerState->GetCharacterType()].Add(CharacterSelectPlayerState);
				AvailableTypes.Remove(CharacterSelectPlayerState->GetCharacterType());
			}
		}

		for (const auto& [CharacterType, Players] : EDMCharacterTypesMap)
		{
			if (Players.Num() > 1)
			{
				EDMDuplicatedPlayers.Append(Players);
				EDMAvailableCharacterTypes.AddUnique(CharacterType);
				NET_LOG(this, Warning, TEXT("EDM Character Type [%s] has duplicated players."), *UEnum::GetValueAsString(CharacterType))
			}
		}

		for (const auto& [CharacterType, Players] : FBCharacterTypesMap)
		{
			if (Players.Num() > 1)
			{
				FBDuplicatedPlayers.Append(Players);
				FBAvailableCharacterTypes.AddUnique(CharacterType);
				NET_LOG(this, Warning, TEXT("FB Character Type [%s] has duplicated players."), *UEnum::GetValueAsString(CharacterType))
			}
		}

		EDMDuplicatedPlayers.Sort([](const ASMCharacterSelectPlayerState& A, const ASMCharacterSelectPlayerState& B) {
			return A.GetActorGuid() < B.GetActorGuid();
		});

		FBDuplicatedPlayers.Sort([](const ASMCharacterSelectPlayerState& A, const ASMCharacterSelectPlayerState& B) {
			return A.GetActorGuid() < B.GetActorGuid();
		});

		for (int32 i = 0; i < EDMDuplicatedPlayers.Num(); i++)
		{
			if (i >= EDMAvailableCharacterTypes.Num())
			{
				EDMDuplicatedPlayers[i]->ChangeCharacterType(ESMCharacterType::ElectricGuitar);
				continue;
			}
			EDMDuplicatedPlayers[i]->ChangeCharacterType(EDMAvailableCharacterTypes[i]);
			NET_LOG(this, Warning, TEXT("Player %s automatically changed to %s"), *EDMDuplicatedPlayers[i]->GetPlayerName(), *UEnum::GetValueAsString(EDMAvailableCharacterTypes[i]))
		}

		for (int32 i = 0; i < FBDuplicatedPlayers.Num(); i++)
		{
			if (i >= FBAvailableCharacterTypes.Num())
			{
				FBDuplicatedPlayers[i]->ChangeCharacterType(ESMCharacterType::ElectricGuitar);
				continue;
			}
			FBDuplicatedPlayers[i]->ChangeCharacterType(FBAvailableCharacterTypes[i]);
			NET_LOG(this, Warning, TEXT("Player %s automatically changed to %s"), *FBDuplicatedPlayers[i]->GetPlayerName(), *UEnum::GetValueAsString(FBAvailableCharacterTypes[i]))
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
