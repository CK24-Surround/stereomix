// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixGameplayAbility_Stun.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/StereoMixAbilitySystemComponent.h"
#include "Characters/StereoMixPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Utilities/StereoMixTag.h"

UStereoMixGameplayAbility_Stun::UStereoMixGameplayAbility_Stun()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	ActivationOwnedTags = FGameplayTagContainer(StereoMixTag::Character::State::Stun);

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = StereoMixTag::Event::Character::Stun;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UStereoMixGameplayAbility_Stun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UStereoMixAbilitySystemComponent* SourceASC = GetStereoMixAbilitySystemComponentFromActorInfo();
	if (ensure(SourceASC))
	{
		// 몽타주가 정상적으로 실행되면 0.0f 이상의 값을 반환합니다.
		const float Duration = SourceASC->PlayMontage(this, ActivationInfo, StunMontage, 1.0f);
		if (ensure(Duration > 0.0f))
		{
			// 컨트롤 로테이션을 따라 회전하지 않도록 잠급니다.
			AStereoMixPlayerCharacter* SourceCharacter = GetStereoMixPlayerCharacterFromActorInfo();
			if (ensure(SourceCharacter))
			{
				SourceCharacter->SetUseControllerRotation(false);
			}

			// 스턴 시간 만큼 기다립니다.
			UAbilityTask_WaitDelay* WaitDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, StunTime);
			if (ensure(WaitDelayTask))
			{
				CommitAbility(Handle, ActorInfo, ActivationInfo);

				WaitDelayTask->OnFinish.AddDynamic(this, &UStereoMixGameplayAbility_Stun::OnStunTimeEnded);
				WaitDelayTask->ReadyForActivation();
				return;
			}
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}

void UStereoMixGameplayAbility_Stun::OnStunTimeEnded()
{
	const UStereoMixAbilitySystemComponent* SourceASC = GetStereoMixAbilitySystemComponentFromActorInfo();
	if (!ensure(SourceASC))
	{
		return;
	}

	// 스매시 당하는 중에는 기절에서 풀리면 안됩니다. 스매시 마무리 후 기절에서 풀려야합니다.
	if (SourceASC->HasMatchingGameplayTag(StereoMixTag::Character::State::Smashed))
	{
		// 스매시 이벤트를 기다립니다.
		UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, StereoMixTag::Event::Character::OnSmash);
		if (ensure(WaitGameplayEventTask))
		{
			WaitGameplayEventTask->EventReceived.AddDynamic(this, &UStereoMixGameplayAbility_Stun::OnSmash);
			WaitGameplayEventTask->ReadyForActivation();
		}

		return;
	}
	// 잡힌 상태인 경우입니다.
	else if (SourceASC->HasMatchingGameplayTag(StereoMixTag::Character::State::Caught))
	{
		// 잡기를 풉니다.
		CaughtExit();

		UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("StandUp"), CaughtExitMontage, 1.0f);
		if (ensure(PlayMontageAndWaitTask))
		{
			PlayMontageAndWaitTask->OnBlendOut.AddDynamic(this, &UStereoMixGameplayAbility_Stun::OnComplete);
			PlayMontageAndWaitTask->OnInterrupted.AddDynamic(this, &UStereoMixGameplayAbility_Stun::OnInterrupted);
			PlayMontageAndWaitTask->OnCancelled.AddDynamic(this, &UStereoMixGameplayAbility_Stun::OnInterrupted);
			PlayMontageAndWaitTask->ReadyForActivation();
		}

		return;
	}
	// 기절 상태인 경우입니다. 일반적인 상황입니다.
	else
	{
		// 기상합니다.
		StunExit();

		UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("StandUp"), StandUpMontage, 1.0f);
		if (PlayMontageAndWaitTask)
		{
			PlayMontageAndWaitTask->OnCompleted.AddDynamic(this, &UStereoMixGameplayAbility_Stun::OnComplete);
			PlayMontageAndWaitTask->OnInterrupted.AddDynamic(this, &UStereoMixGameplayAbility_Stun::OnInterrupted);
			PlayMontageAndWaitTask->OnCancelled.AddDynamic(this, &UStereoMixGameplayAbility_Stun::OnInterrupted);
			PlayMontageAndWaitTask->ReadyForActivation();
		}

		return;
	}
}

void UStereoMixGameplayAbility_Stun::OnSmash(FGameplayEventData Payload)
{
	// 잡기 상태를 해제합니다.
	CaughtExit();

	UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("StandUp"), StandUpMontage, 1.0f);
	if (ensure(PlayMontageAndWaitTask))
	{
		PlayMontageAndWaitTask->OnBlendOut.AddDynamic(this, &UStereoMixGameplayAbility_Stun::OnComplete);
		PlayMontageAndWaitTask->ReadyForActivation();
	}
}

void UStereoMixGameplayAbility_Stun::CaughtExit()
{
	UStereoMixAbilitySystemComponent* SourceASC = GetStereoMixAbilitySystemComponentFromActorInfo();
	if (!ensure(SourceASC))
	{
		return;
	}

	// 아래 로직은 모두 서버에서 수행되야합니다.
	if (CurrentActorInfo->IsNetAuthority())
	{
		// 풀려나고 있는 중도 Stun 상태이기 때문에 잡힐 수 있습니다. 이를 방지하고자 UnCatchable 태그를 사용합니다.
		// 서버에서만 실행하는 이유는 HasMatchingGameplayTag때문에 클라이언트에서 잘못 실행될 가능성이 있기 때문입니다.
		SourceASC->AddLooseGameplayTag(StereoMixTag::Character::State::Uncatchable);
		SourceASC->AddReplicatedLooseGameplayTag(StereoMixTag::Character::State::Uncatchable);

		// Source의 Caught 태그 및 잡고 있는 대상을 제거합니다.
		if (SourceASC->HasMatchingGameplayTag(StereoMixTag::Character::State::Caught))
		{
			SourceASC->RemoveLooseGameplayTag(StereoMixTag::Character::State::Caught);
			SourceASC->RemoveReplicatedLooseGameplayTag(StereoMixTag::Character::State::Caught);
		}

		// Target의 Catch 태그 및 잡혀 있는 대상을 제거합니다.
		AStereoMixPlayerCharacter* TargetCharacter = Cast<AStereoMixPlayerCharacter>(SourceASC->GetCurrentCaughtPawn());
		if (ensure(TargetCharacter))
		{
			UStereoMixAbilitySystemComponent* TargetASC = Cast<UStereoMixAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetCharacter));
			if (ensure(TargetASC))
			{
				if (TargetASC->HasMatchingGameplayTag(StereoMixTag::Character::State::Catch))
				{
					TargetASC->RemoveLooseGameplayTag(StereoMixTag::Character::State::Catch);
					TargetASC->RemoveReplicatedLooseGameplayTag(StereoMixTag::Character::State::Catch);
				}

				// 디태치합니다. 
				DetachToTargetCharacter();

				// 마지막으로 Catch 및 Caught 폰을 초기화해줍니다.
				SourceASC->SetCurrentCaughtPawn(nullptr);
				TargetASC->SetCurrentCatchPawn(nullptr);
			}
		}
	}
}

void UStereoMixGameplayAbility_Stun::DetachToTargetCharacter()
{
	UStereoMixAbilitySystemComponent* SourceASC = GetStereoMixAbilitySystemComponentFromActorInfo();
	if (!ensure(SourceASC))
	{
		return;
	}

	AStereoMixPlayerCharacter* SourceCharacter = GetStereoMixPlayerCharacterFromActorInfo();
	if (ensure(SourceCharacter))
	{
		SourceCharacter->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		// 콜리전, 움직임을 복구합니다.
		SourceCharacter->SetEnableCollision(true);
		SourceCharacter->SetEnableMovement(true);

		// 회전을 재지정합니다.
		const AStereoMixPlayerCharacter* TargetCharacter = Cast<AStereoMixPlayerCharacter>(SourceASC->GetCurrentCaughtPawn());
		if (ensure(TargetCharacter))
		{
			const float TargetYaw = TargetCharacter->GetActorRotation().Yaw;
			MulticastRPCSetRotationWithTarget(TargetYaw);
		}

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
}

void UStereoMixGameplayAbility_Stun::MulticastRPCSetRotationWithTarget_Implementation(float InYaw)
{
	AStereoMixPlayerCharacter* SourceCharacter = GetStereoMixPlayerCharacterFromActorInfo();
	if (ensure(SourceCharacter))
	{
		SourceCharacter->SetActorRotation(FRotator(0.0, InYaw, 0.0));
	}
}

void UStereoMixGameplayAbility_Stun::StunExit()
{
	UStereoMixAbilitySystemComponent* SourceASC = GetStereoMixAbilitySystemComponentFromActorInfo();
	if (!ensure(SourceASC))
	{
		return;
	}

	if (CurrentActorInfo->IsNetAuthority())
	{
		// 기상 중에도 Stun 상태이기 때문에 잡힐 수 있습니다. 이를 방지하고자 UnCatchable 태그를 사용합니다.
		// 서버에서만 실행하는 이유는 HasMatchingGameplayTag때문에 클라이언트에서 잘못 실행될 가능성이 있기 때문입니다.
		SourceASC->AddLooseGameplayTag(StereoMixTag::Character::State::Uncatchable);
		SourceASC->AddReplicatedLooseGameplayTag(StereoMixTag::Character::State::Uncatchable);
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
		// 서버에서만 실행하는 이유는 HasMatchingGameplayTag때문에 클라이언트에서 잘못 실행될 가능성이 있기 때문입니다.
		if (SourceASC->HasMatchingGameplayTag(StereoMixTag::Character::State::Uncatchable))
		{
			// 서버렉으로 인해 종종 제거가 올바르게 되지 않는 경우가 있습니다. 이런 경우 유용합니다.
			while (SourceASC->HasMatchingGameplayTag(StereoMixTag::Character::State::Uncatchable))
			{
				SourceASC->RemoveLooseGameplayTag(StereoMixTag::Character::State::Uncatchable);
				SourceASC->RemoveReplicatedLooseGameplayTag(StereoMixTag::Character::State::Uncatchable);
			}
		}

		for (const auto& StunEndedGE : StunEndedGEs)
		{
			if (ensure(StunEndedGE))
			{
				BP_ApplyGameplayEffectToOwner(StunEndedGE);
			}
		}
	}
}
