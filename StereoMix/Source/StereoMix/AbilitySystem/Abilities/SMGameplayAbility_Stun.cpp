// Fill out your copyright notice in the Description page of Project Settings.


#include "SMGameplayAbility_Stun.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Characters/SMPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Utilities/SMLog.h"
#include "Utilities/SMTags.h"

USMGameplayAbility_Stun::USMGameplayAbility_Stun()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	ActivationOwnedTags = FGameplayTagContainer(SMTags::Character::State::Stun);

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = SMTags::Event::Character::Stun;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void USMGameplayAbility_Stun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	USMAbilitySystemComponent* SourceASC = GetStereoMixAbilitySystemComponentFromActorInfo();
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
	ASMPlayerCharacter* SourceCharacter = GetStereoMixPlayerCharacterFromActorInfo();
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

	WaitDelayTask->OnFinish.AddDynamic(this, &USMGameplayAbility_Stun::OnStunTimeEnded);
	WaitDelayTask->ReadyForActivation();

	CommitAbility(Handle, ActorInfo, ActivationInfo);
}

void USMGameplayAbility_Stun::OnStunTimeEnded()
{
	const USMAbilitySystemComponent* SourceASC = GetStereoMixAbilitySystemComponentFromActorInfo();
	if (!ensure(SourceASC))
	{
		return;
	}

	// 스매시 당하는 상태인 경우의 처리입니다.
	if (SourceASC->HasMatchingGameplayTag(SMTags::Character::State::Smashed))
	{
		ProcessBuzzerBeaterSmashed();
		return;
	}
	// 잡힌 상태인 경우의 처리입니다.
	else if (SourceASC->HasMatchingGameplayTag(SMTags::Character::State::Caught))
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

void USMGameplayAbility_Stun::ProcessBuzzerBeaterSmashed()
{
	NET_LOG(GetStereoMixPlayerCharacterFromActorInfo(), Log, TEXT("버저 비터 상태 진입"));
	// 스매시 이벤트를 기다립니다.
	UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::Character::BuzzerBeaterSmashEnd);
	if (ensure(WaitGameplayEventTask))
	{
		WaitGameplayEventTask->EventReceived.AddDynamic(this, &USMGameplayAbility_Stun::OnBuzzerBeaterSmashEnded);
		WaitGameplayEventTask->ReadyForActivation();
	}
}

void USMGameplayAbility_Stun::OnBuzzerBeaterSmashEnded(FGameplayEventData Payload)
{
	NET_LOG(GetStereoMixPlayerCharacterFromActorInfo(), Log, TEXT("버저 비터 종료"));
	UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("StandUp"), StandUpMontage, 1.0f);
	if (ensure(PlayMontageAndWaitTask))
	{
		PlayMontageAndWaitTask->OnBlendOut.AddDynamic(this, &USMGameplayAbility_Stun::OnComplete);
		PlayMontageAndWaitTask->ReadyForActivation();
	}
}

void USMGameplayAbility_Stun::ResetCaughtState()
{
	NET_LOG(GetStereoMixPlayerCharacterFromActorInfo(), Log, TEXT("잡기 탈출"));
	USMAbilitySystemComponent* SourceASC = GetStereoMixAbilitySystemComponentFromActorInfo();
	if (!ensure(SourceASC))
	{
		return;
	}

	if (CurrentActorInfo->IsNetAuthority())
	{
		// 풀려나고 있는 중도 Stun 상태이기 때문에 잡힐 수 있습니다. 이를 방지하고자 UnCatchable 태그를 사용합니다.
		SourceASC->AddTag(SMTags::Character::State::Uncatchable);

		// Source의 Caught 태그 및 잡고 있는 대상을 제거합니다.
		SourceASC->RemoveTag(SMTags::Character::State::Caught);

		ASMPlayerCharacter* SourceCharacter = GetStereoMixPlayerCharacterFromActorInfo();
		if (ensure(SourceCharacter))
		{
			ASMPlayerCharacter* TargetCharacter = SourceCharacter->GetCaughtCharacter();
			if (ensure(TargetCharacter))
			{
				USMAbilitySystemComponent* TargetASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetCharacter));
				if (ensure(TargetASC))
				{
					// Target의 Catch 태그 및 잡혀 있는 대상을 제거합니다.
					TargetASC->RemoveTag(SMTags::Character::State::Catch);

					// 디태치와 필요한 처리를 해줍니다.
					DetachFromTargetCharacter(TargetCharacter);
				}

				// 마지막으로 Catch 및 Caught 폰을 초기화해줍니다.
				SourceCharacter->SetCaughtCharacter(nullptr);
				TargetCharacter->SetCatchCharacter(nullptr);
			}
		}
	}

	UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("CatchExit"), CatchExitMontage, 1.0f);
	if (ensure(PlayMontageAndWaitTask))
	{
		PlayMontageAndWaitTask->OnBlendOut.AddDynamic(this, &USMGameplayAbility_Stun::OnComplete);
		PlayMontageAndWaitTask->ReadyForActivation();
	}
}

void USMGameplayAbility_Stun::DetachFromTargetCharacter(ASMPlayerCharacter* InTargetCharacter)
{
	if (!ensure(InTargetCharacter))
	{
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetStereoMixAbilitySystemComponentFromActorInfo();
	if (!ensure(SourceASC))
	{
		return;
	}

	ASMPlayerCharacter* SourceCharacter = GetStereoMixPlayerCharacterFromActorInfo();
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

void USMGameplayAbility_Stun::ResetStunState()
{
	NET_LOG(GetStereoMixPlayerCharacterFromActorInfo(), Log, TEXT("스턴 종료"));
	USMAbilitySystemComponent* SourceASC = GetStereoMixAbilitySystemComponentFromActorInfo();
	if (!ensure(SourceASC))
	{
		return;
	}

	if (CurrentActorInfo->IsNetAuthority())
	{
		// 기상 중에도 Stun 상태이기 때문에 잡힐 수 있습니다. 이를 방지하고자 UnCatchable 태그를 사용합니다.
		SourceASC->AddTag(SMTags::Character::State::Uncatchable);
	}

	UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("StunEnd"), StunEndMontage, 1.0f);
	if (ensure(PlayMontageAndWaitTask))
	{
		PlayMontageAndWaitTask->OnBlendOut.AddDynamic(this, &USMGameplayAbility_Stun::OnComplete);
		PlayMontageAndWaitTask->ReadyForActivation();
	}
}

void USMGameplayAbility_Stun::OnComplete()
{
	OnStunEnded();

	// 2루트로 나눈 이유는 만약 클라이언트에서 먼저 EndAbility에 도달한다면 서버의 로직이 끝나지 않아도 서버의 어빌리티를 종료시켜 문제가 발생하기 때문입니다.
	if (CurrentActorInfo->IsNetAuthority())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
	}
}

void USMGameplayAbility_Stun::OnStunEnded()
{
	USMAbilitySystemComponent* SourceASC = GetStereoMixAbilitySystemComponentFromActorInfo();
	if (!ensure(SourceASC))
	{
		return;
	}

	if (CurrentActorInfo->IsNetAuthority())
	{
		// 컨트롤 로테이션을 따라가도록 복구해줍니다.
		ASMPlayerCharacter* SourceCharacter = GetStereoMixPlayerCharacterFromActorInfo();
		if (ensure(SourceCharacter))
		{
			SourceCharacter->SetUseControllerRotation(true);
		}

		// 스턴이 완전히 종료되었기에 Uncatchable 태그를 제거합니다.
		SourceASC->RemoveTag(SMTags::Character::State::Uncatchable);

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
