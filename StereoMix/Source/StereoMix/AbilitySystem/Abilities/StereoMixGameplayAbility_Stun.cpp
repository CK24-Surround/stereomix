// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixGameplayAbility_Stun.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/StereoMixAbilitySystemComponent.h"
#include "Characters/StereoMixPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Utilities/StereoMixLog.h"
#include "Utilities/StereoMixTagName.h"

UStereoMixGameplayAbility_Stun::UStereoMixGameplayAbility_Stun()
{
	CatchStateTag = FGameplayTag::RequestGameplayTag(StereoMixTagName::Character::State::Catch);
	CaughtStateTag = FGameplayTag::RequestGameplayTag(StereoMixTagName::Character::State::Caught);
	SmashedStateTag = FGameplayTag::RequestGameplayTag(StereoMixTagName::Character::State::Smashed);
	OnSmashEventTag = FGameplayTag::RequestGameplayTag(StereoMixTagName::Character::State::Smashed);
	UncatchableStateTag = FGameplayTag::RequestGameplayTag(StereoMixTagName::Character::State::Uncatchable);
	
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	ActivationOwnedTags = FGameplayTagContainer(FGameplayTag::RequestGameplayTag(StereoMixTagName::Character::State::Stun));

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = FGameplayTag::RequestGameplayTag(StereoMixTagName::Event::Character::Stun);
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UStereoMixGameplayAbility_Stun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UStereoMixAbilitySystemComponent* SourceASC = GetStereoMixAbilitySystemComponentFromActorInfo();
	if (!ensure(SourceASC))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 스턴 몽타주가 정상적으로 실행되면 0.0f 이상의 값을 반환합니다.
	const float Duration = SourceASC->PlayMontage(this, ActivationInfo, StunMontage, 1.0f);
	if (!ensure(Duration > 0.0f))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 컨트롤 로테이션을 따라 회전하지 않도록 잠급니다.
	AStereoMixPlayerCharacter* SourceCharacter = GetStereoMixPlayerCharacterFromActorInfo();
	if (ensure(SourceCharacter))
	{
		SourceCharacter->SetUseControllerRotation(false);
	}

	// 스턴 시간 만큼 기다립니다.
	UAbilityTask_WaitDelay* WaitDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, StunTime);
	if (!ensure(WaitDelayTask))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	WaitDelayTask->OnFinish.AddDynamic(this, &UStereoMixGameplayAbility_Stun::OnStunTimeEnded);
	WaitDelayTask->ReadyForActivation();

	CommitAbility(Handle, ActorInfo, ActivationInfo);
}

void UStereoMixGameplayAbility_Stun::OnStunTimeEnded()
{
	const UStereoMixAbilitySystemComponent* SourceASC = GetStereoMixAbilitySystemComponentFromActorInfo();
	if (!ensure(SourceASC))
	{
		return;
	}

	int32 TagCount = SourceASC->GetGameplayTagCount(CaughtStateTag);
	NET_LOG(GetStereoMixPlayerCharacterFromActorInfo(), Log, TEXT("잡힌 상태 태그 수: %d"), TagCount);

	// 스매시 당하는 상태인 경우의 처리입니다.
	if (SourceASC->HasMatchingGameplayTag(SmashedStateTag))
	{
		ResetSmashedState();
		return;
	}
	// 잡힌 상태인 경우의 처리입니다.
	else if (SourceASC->HasMatchingGameplayTag(CaughtStateTag))
	{
		ResetCaughtState();
		return;
	}
	// 기절 상태인 경우의 처리입니다.
	else
	{
		ResetStunState();
		return;
	}
}

void UStereoMixGameplayAbility_Stun::ResetSmashedState()
{
	// 스매시 이벤트를 기다립니다.
	UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, OnSmashEventTag);
	if (ensure(WaitGameplayEventTask))
	{
		WaitGameplayEventTask->EventReceived.AddDynamic(this, &UStereoMixGameplayAbility_Stun::OnSmashEnded);
		WaitGameplayEventTask->ReadyForActivation();
	}
}

void UStereoMixGameplayAbility_Stun::OnSmashEnded(FGameplayEventData Payload)
{
	NET_LOG(GetStereoMixPlayerCharacterFromActorInfo(), Log, TEXT("스매시 종료 이벤트 콜"));

	UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("StandUp"), StandUpMontage, 1.0f);
	if (ensure(PlayMontageAndWaitTask))
	{
		PlayMontageAndWaitTask->OnBlendOut.AddDynamic(this, &UStereoMixGameplayAbility_Stun::OnComplete);
		PlayMontageAndWaitTask->ReadyForActivation();
	}
}

void UStereoMixGameplayAbility_Stun::ResetCaughtState()
{
	UStereoMixAbilitySystemComponent* SourceASC = GetStereoMixAbilitySystemComponentFromActorInfo();
	if (!ensure(SourceASC))
	{
		return;
	}

	if (CurrentActorInfo->IsNetAuthority())
	{
		// 풀려나고 있는 중도 Stun 상태이기 때문에 잡힐 수 있습니다. 이를 방지하고자 UnCatchable 태그를 사용합니다.
		SourceASC->AddTag(UncatchableStateTag);

		// Source의 Caught 태그 및 잡고 있는 대상을 제거합니다.
		SourceASC->RemoveTag(CaughtStateTag);

		AStereoMixPlayerCharacter* SourceCharacter = GetStereoMixPlayerCharacterFromActorInfo();
		if (ensure(SourceCharacter))
		{
			AStereoMixPlayerCharacter* TargetCharacter = SourceCharacter->GetCaughtCharacter();
			if (ensure(TargetCharacter))
			{
				UStereoMixAbilitySystemComponent* TargetASC = Cast<UStereoMixAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetCharacter));
				if (ensure(TargetASC))
				{
					// Target의 Catch 태그 및 잡혀 있는 대상을 제거합니다.
					TargetASC->RemoveTag(CatchStateTag);

					// 디태치와 필요한 처리를 해줍니다.
					DetachFromTargetCharacter(TargetCharacter);
				}

				// 마지막으로 Catch 및 Caught 폰을 초기화해줍니다.
				SourceCharacter->SetCaughtCharacter(nullptr);
				TargetCharacter->SetCatchCharacter(nullptr);
			}
		}
	}

	UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("StandUp"), CaughtExitMontage, 1.0f);
	if (ensure(PlayMontageAndWaitTask))
	{
		PlayMontageAndWaitTask->OnBlendOut.AddDynamic(this, &UStereoMixGameplayAbility_Stun::OnComplete);
		PlayMontageAndWaitTask->OnInterrupted.AddDynamic(this, &UStereoMixGameplayAbility_Stun::OnInterrupted);
		PlayMontageAndWaitTask->OnCancelled.AddDynamic(this, &UStereoMixGameplayAbility_Stun::OnInterrupted);
		PlayMontageAndWaitTask->ReadyForActivation();
	}
}

void UStereoMixGameplayAbility_Stun::DetachFromTargetCharacter(AStereoMixPlayerCharacter* InTargetCharacter)
{
	if (!ensure(InTargetCharacter))
	{
		return;
	}

	UStereoMixAbilitySystemComponent* SourceASC = GetStereoMixAbilitySystemComponentFromActorInfo();
	if (!ensure(SourceASC))
	{
		return;
	}

	AStereoMixPlayerCharacter* SourceCharacter = GetStereoMixPlayerCharacterFromActorInfo();
	if (!ensure(SourceCharacter))
	{
		return;
	}

	SourceCharacter->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	// 콜리전, 움직임을 복구합니다.
	SourceCharacter->SetEnableCollision(true);
	SourceCharacter->SetEnableMovement(true);

	// 회전을 재지정합니다.
	const float TargetYaw = InTargetCharacter->GetActorRotation().Yaw;
	SourceCharacter->MulticastRPCSetYawRotation(TargetYaw);

	UCharacterMovementComponent* SourceMovement = SourceCharacter->GetCharacterMovement();
	if (ensure(SourceMovement))
	{
		SourceMovement->bIgnoreClientMovementErrorChecksAndCorrection = false;
	}

	APlayerController* SourcePlayerController = Cast<APlayerController>(SourceCharacter->GetController());
	if (ensure(SourcePlayerController))
	{
		SourcePlayerController->SetViewTargetWithBlend(SourceCharacter, 1.0f, VTBlend_Cubic);
	}
}

void UStereoMixGameplayAbility_Stun::ResetStunState()
{
	UStereoMixAbilitySystemComponent* SourceASC = GetStereoMixAbilitySystemComponentFromActorInfo();
	if (!ensure(SourceASC))
	{
		return;
	}

	if (CurrentActorInfo->IsNetAuthority())
	{
		// 기상 중에도 Stun 상태이기 때문에 잡힐 수 있습니다. 이를 방지하고자 UnCatchable 태그를 사용합니다.
		SourceASC->AddTag(UncatchableStateTag);
	}

	UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("StandUp"), StandUpMontage, 1.0f);
	if (ensure(PlayMontageAndWaitTask))
	{
		PlayMontageAndWaitTask->OnBlendOut.AddDynamic(this, &UStereoMixGameplayAbility_Stun::OnComplete);
		PlayMontageAndWaitTask->OnInterrupted.AddDynamic(this, &UStereoMixGameplayAbility_Stun::OnInterrupted);
		PlayMontageAndWaitTask->OnCancelled.AddDynamic(this, &UStereoMixGameplayAbility_Stun::OnInterrupted);
		PlayMontageAndWaitTask->ReadyForActivation();
	}
}

void UStereoMixGameplayAbility_Stun::OnComplete()
{
	OnStunEnded();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UStereoMixGameplayAbility_Stun::OnInterrupted()
{
	OnStunEnded();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UStereoMixGameplayAbility_Stun::OnStunEnded()
{
	UStereoMixAbilitySystemComponent* SourceASC = GetStereoMixAbilitySystemComponentFromActorInfo();
	if (!ensure(SourceASC))
	{
		return;
	}

	if (CurrentActorInfo->IsNetAuthority())
	{
		// 컨트롤 로테이션을 따라가도록 복구해줍니다.
		AStereoMixPlayerCharacter* SourceCharacter = GetStereoMixPlayerCharacterFromActorInfo();
		if (ensure(SourceCharacter))
		{
			SourceCharacter->SetUseControllerRotation(true);
		}

		// 스턴이 완전히 종료되었기에 Uncatchable 태그를 제거합니다.
		SourceASC->RemoveTag(UncatchableStateTag);

		// 마지막으로 적용해야할 GE들을 모두 적용합니다.
		for (const auto& StunEndedGE : StunEndedGEs)
		{
			if (ensure(StunEndedGE))
			{
				BP_ApplyGameplayEffectToOwner(StunEndedGE);
			}
		}
	}
}
