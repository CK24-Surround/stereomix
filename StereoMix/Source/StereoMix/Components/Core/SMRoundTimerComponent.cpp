// Copyright Studio Surround. All Rights Reserved.


#include "SMRoundTimerComponent.h"

#include "Net/UnrealNetwork.h"
#include "Utilities/SMLog.h"


USMRoundTimerComponent::USMRoundTimerComponent()
{
	// PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void USMRoundTimerComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, TimerState);
	DOREPLIFETIME(ThisClass, RemainingTime);
}

void USMRoundTimerComponent::StartTimer()
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

	NET_LOG(GetOwner(), Log, TEXT("프리 라운드 시작"));
	SetRemainingTime(PreRoundTime);
	World->GetTimerManager().SetTimer(CountdownTimerHandle, this, &ThisClass::CountdownTime, OneSecond, true);
}

void USMRoundTimerComponent::CountdownTime()
{
	SetRemainingTime(RemainingTime - 1);

	if (RemainingTime <= 0)
	{
		switch (TimerState)
		{
			case ESMTimerState::PreRound:
			{
				NET_LOG(GetOwner(), Log, TEXT("라운드 시작"));
				TimerState = ESMTimerState::InRound;
				SetRemainingTime(RoundTime);
				OnPreRoundTimeExpired.Broadcast();
				break;
			}
			case ESMTimerState::InRound:
			{
				NET_LOG(GetOwner(), Log, TEXT("포스트 라운드 시작"));
				TimerState = ESMTimerState::PostRound;
				SetRemainingTime(PostRoundTime);
				OnRoundTimeExpired.Broadcast();
				break;
			}
			case ESMTimerState::PostRound:
			{
				if (const UWorld* World = GetWorld())
				{
					World->GetTimerManager().ClearTimer(CountdownTimerHandle);
				}

				OnPostRoundTimeExpired.Broadcast();
				break;
			}
		}
	}
}

void USMRoundTimerComponent::SetRemainingTime(int32 NewTime)
{
	RemainingTime = NewTime;

	OnRep_RemainingTime();
}

void USMRoundTimerComponent::OnRep_RemainingTime()
{
	OnRemainingRoundTimeChanged.Broadcast(RemainingTime);
}
