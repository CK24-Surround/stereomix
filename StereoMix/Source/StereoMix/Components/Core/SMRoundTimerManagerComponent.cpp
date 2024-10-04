// Copyright Studio Surround. All Rights Reserved.


#include "SMRoundTimerManagerComponent.h"

#include "GameFramework/GameMode.h"
#include "SMTileManagerComponent.h"
#include "Games/SMGameState.h"
#include "Net/UnrealNetwork.h"
#include "Utilities/SMLog.h"


USMRoundTimerManagerComponent::USMRoundTimerManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	SetIsReplicatedByDefault(true);
}

void USMRoundTimerManagerComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, RemainingTime);
}

void USMRoundTimerManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (GetOwnerRole() == ROLE_Authority)
	{
		if (ASMGameState* GameState = GetOwner<ASMGameState>())
		{
			GameState->OnPreRoundStart.AddDynamic(this, &ThisClass::StartTimer);
		}
	}
}

void USMRoundTimerManagerComponent::StartTimer()
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		return;
	}

	UWorld* World = GetWorld();
	AWorldSettings* WorldSettings = World ? World->GetWorldSettings() : nullptr;
	const float OneSecond = WorldSettings ? WorldSettings->GetEffectiveTimeDilation() : 1.0f;
	if (!World)
	{
		return;
	}

	NET_LOG(GetOwner(), Warning, TEXT("프리 라운드 시작"));
	SetRemainingTime(PreRoundTime);
	World->GetTimerManager().SetTimer(CountdownTimerHandle, this, &ThisClass::CountdownTime, OneSecond, true);
}

void USMRoundTimerManagerComponent::CountdownTime()
{
	SetRemainingTime(RemainingTime - 1);

	if (RemainingTime <= 0)
	{
		const UWorld* World = GetWorld();
		ASMGameState* GameState = GetOwner<ASMGameState>();
		if (World && GameState)
		{
			switch (GameState->GetRoundState())
			{
				case ESMRoundState::PreRound:
				{
					NET_LOG(GetOwner(), Log, TEXT("라운드 시작"));
					GameState->SetRoundState(ESMRoundState::InRound);
					SetRemainingTime(RoundTime);

					if (AGameMode* GameMode = World->GetAuthGameMode<AGameMode>())
					{
						GameMode->StartMatch();
					}

					OnPreRoundTimeExpired.Broadcast();
					break;
				}
				case ESMRoundState::InRound:
				{
					NET_LOG(GetOwner(), Log, TEXT("포스트 라운드 시작"));
					GameState->SetRoundState(ESMRoundState::PostRound);
					SetRemainingTime(PostRoundTime);

					if (AGameMode* GameMode = World->GetAuthGameMode<AGameMode>())
					{
						GameMode->EndMatch();
					}

					if (USMTileManagerComponent* TileManager = GameState->GetComponentByClass<USMTileManagerComponent>())
					{
						TileManager->ShowGameResult();
					}

					OnRoundTimeExpired.Broadcast();
					break;
				}
				case ESMRoundState::PostRound:
				{
					NET_LOG(GetOwner(), Log, TEXT("라운드 종료"));
					GameState->SetRoundState(ESMRoundState::None);

					if (World)
					{
						World->GetTimerManager().ClearTimer(CountdownTimerHandle);
					}

					if (AGameMode* GameMode = World->GetAuthGameMode<AGameMode>())
					{
						GameMode->StartToLeaveMap();
					}

					OnPostRoundTimeExpired.Broadcast();
					break;
				}
				default:
				{
					break;
				}
			}
		}
	}
}

void USMRoundTimerManagerComponent::SetRemainingTime(int32 NewTime)
{
	RemainingTime = NewTime;

	OnRep_RemainingTime();
}

void USMRoundTimerManagerComponent::OnRep_RemainingTime()
{
	OnRemainingRoundTimeChanged.Broadcast(RemainingTime);
}
