// Copyright Surround, Inc. All Rights Reserved.


#include "CountdownTimerComponent.h"
#include "Net/UnrealNetwork.h"
#include "Utilities/SMLog.h"


UCountdownTimerComponent::UCountdownTimerComponent()
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = false;
}

void UCountdownTimerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCountdownTimerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (const UWorld* CurrentWorld = GetWorld())
	{
		if (TimerHandle.IsValid())
		{
			CurrentWorld->GetTimerManager().ClearTimer(TimerHandle);
		}
	}
}

void UCountdownTimerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InitTime)
	DOREPLIFETIME(ThisClass, RemainingTime)
}

void UCountdownTimerComponent::OnRep_RemainingTime() const
{
	NET_LOG(GetOwner(), Verbose, TEXT("Remaining time: %d"), RemainingTime)
	if (OnCountdownTick.IsBound())
	{
		OnCountdownTick.Broadcast();
	}
}

void UCountdownTimerComponent::PerformCountdownTick()
{
	--RemainingTime;
	if (RemainingTime <= 0)
	{
		NET_LOG(GetOwner(), Verbose, TEXT("Countdown finished."))
		if (const UWorld* CurrentWorld = GetWorld())
		{
			CurrentWorld->GetTimerManager().ClearTimer(TimerHandle);
			TimerHandle.Invalidate();
		}
		if (OnCountdownFinished.IsBound())
		{
			OnCountdownFinished.Broadcast();
		}
	}
	else
	{
		if (OnCountdownTick.IsBound())
		{
			OnCountdownTick.Broadcast();
		}
	}
}

void UCountdownTimerComponent::StartCountdown(const int32 Seconds)
{
	if (TimerHandle.IsValid())
	{
		NET_LOG(GetOwner(), Warning, TEXT("Countdown already running."))
	}

	InitTime = Seconds;
	RemainingTime = Seconds;
	if (const UWorld* CurrentWorld = GetWorld())
	{
		CurrentWorld->GetTimerManager().SetTimer(TimerHandle, this, &UCountdownTimerComponent::PerformCountdownTick, 1.0f, true);
		NET_LOG(GetOwner(), Verbose, TEXT("Countdown started."))
	}
	else
	{
		NET_LOG(GetOwner(), Error, TEXT("Failed to get world."))
	}
}

void UCountdownTimerComponent::CancelCountdown()
{
	if (!TimerHandle.IsValid())
	{
		return;
	}

	if (const UWorld* CurrentWorld = GetWorld())
	{
		NET_LOG(GetOwner(), Verbose, TEXT("Countdown cancelled."))
		CurrentWorld->GetTimerManager().ClearTimer(TimerHandle);
		TimerHandle.Invalidate();
		if (OnCountdownCancelled.IsBound())
		{
			OnCountdownCancelled.Broadcast();
		}
	}
}
