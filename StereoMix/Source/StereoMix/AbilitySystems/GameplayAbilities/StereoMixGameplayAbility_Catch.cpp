// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixGameplayAbility_Catch.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilityTasks/StereoMixAbilityTask_Trace.h"
#include "Characters/StereoMixPlayerCharacter.h"
#include "Utilities/StereoMixeLog.h"
#include "Utilities/StereoMixTag.h"

UStereoMixGameplayAbility_Catch::UStereoMixGameplayAbility_Catch()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
}

void UStereoMixGameplayAbility_Catch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!ensure(ActorInfo))
	{
		return;
	}

	AStereoMixPlayerCharacter* StereoMixCharacter = Cast<AStereoMixPlayerCharacter>(ActorInfo->AvatarActor.Get());
	if (StereoMixCharacter) {}

	UAbilityTask_PlayMontageAndWait* AbilityTaskPlayMontageAndWait = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayMontageAndWait"), CatchMontage);
	if (AbilityTaskPlayMontageAndWait)
	{
		AbilityTaskPlayMontageAndWait->OnCompleted.AddDynamic(this, &UStereoMixGameplayAbility_Catch::OnFinished);
		AbilityTaskPlayMontageAndWait->OnCancelled.AddDynamic(this, &UStereoMixGameplayAbility_Catch::OnCancelled);
		AbilityTaskPlayMontageAndWait->OnInterrupted.AddDynamic(this, &UStereoMixGameplayAbility_Catch::OnInterrupted);
		AbilityTaskPlayMontageAndWait->ReadyForActivation();
	}

	CommitAbility(Handle, ActorInfo, ActivationInfo);

	if (!ActorInfo->IsNetAuthority())
	{
		UAbilityTask_WaitGameplayEvent* AbilityTaskWaitGameplayEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, StereoMixTag::Event::Character::Catch);
		if (AbilityTaskWaitGameplayEvent)
		{
			AbilityTaskWaitGameplayEvent->EventReceived.AddDynamic(this, &UStereoMixGameplayAbility_Catch::OnEventReceived);
			AbilityTaskWaitGameplayEvent->ReadyForActivation();
		}
	}
}

void UStereoMixGameplayAbility_Catch::OnFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UStereoMixGameplayAbility_Catch::OnCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UStereoMixGameplayAbility_Catch::OnInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UStereoMixGameplayAbility_Catch::OnEventReceived(FGameplayEventData Payload)
{
	if (!CurrentActorInfo)
	{
		return;
	}

	AStereoMixPlayerCharacter* StereoMixCharacter = Cast<AStereoMixPlayerCharacter>(CurrentActorInfo->AvatarActor.Get());
	if (!StereoMixCharacter)
	{
		return;
	}

	const FVector StartLocation = StereoMixCharacter->GetActorLocation();
	const FVector TargetLocation = StereoMixCharacter->GetCursorTargetingPoint();
	ServerRPCRequestSpawnTargetActor(StartLocation, TargetLocation);
}

void UStereoMixGameplayAbility_Catch::ServerRPCRequestSpawnTargetActor_Implementation(const FVector_NetQuantize10& StartLocation, const FVector_NetQuantize10& CursorLocation)
{
	if (StartLocation.Z != CursorLocation.Z)
	{
		NET_LOG(CurrentActorInfo ? CurrentActorInfo->AvatarActor.Get() : nullptr, Warning, TEXT("캐릭터의 위치와 커서의 위치가 평면상에 놓이지 않았습니다. 투사체 시작점: %s 목표 위치: %s"), *StartLocation.ToString(), *CursorLocation.ToString());
	}

	const FVector Direction = (CursorLocation - StartLocation).GetSafeNormal();
	UStereoMixAbilityTask_Trace* UStereoMixAbilityTaskTrace = UStereoMixAbilityTask_Trace::CreateTask(this, StartLocation, Direction);
	if (UStereoMixAbilityTaskTrace)
	{
		UStereoMixAbilityTaskTrace->OnCompleteTrace.AddDynamic(this, &UStereoMixGameplayAbility_Catch::OnCompleteTrace);
		UStereoMixAbilityTaskTrace->ReadyForActivation();
	}
}

void UStereoMixGameplayAbility_Catch::OnCompleteTrace(const FGameplayAbilityTargetDataHandle& TargetDataHandle) {}
