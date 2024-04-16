// Fill out your copyright notice in the Description page of Project Settings.


#include "SMGameplayAbility_Stun.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Characters/SMPlayerCharacter.h"
#include "Utilities/SMLog.h"
#include "Utilities/SMTags.h"

USMGameplayAbility_Stun::USMGameplayAbility_Stun()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

	ActivationOwnedTags = FGameplayTagContainer(SMTags::Character::State::Stun);

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = SMTags::Event::Character::Stun;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void USMGameplayAbility_Stun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensure(SourceASC))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 스턴 몽타주가 정상적으로 실행되면 0.0f 이상의 값을 반환합니다.
	const float Duration = SourceASC->PlayMontage(this, ActivationInfo, StunMontage, 1.0f);
	ClientRPCPlayMontage(StunMontage);
	if (!ensure(Duration > 0.0f))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 컨트롤 로테이션을 따라 회전하지 않도록 잠급니다.
	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
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

	// 잡고 있던 캐릭터가 있다면 놓아줍니다.
	if (SourceASC->HasMatchingGameplayTag(SMTags::Character::State::Catch))
	{
		ASMPlayerCharacter* TargetCharacter = SourceCharacter->GetCatchCharacter();
		if (ensure(TargetCharacter))
		{
			UAbilitySystemComponent* TargetASC = TargetCharacter->GetAbilitySystemComponent();
			if (ensure(TargetASC))
			{
				TargetASC->TryActivateAbilitiesByTag(FGameplayTagContainer(SMTags::Ability::CaughtExit));
			}
		}
	}

	WaitDelayTask->OnFinish.AddDynamic(this, &USMGameplayAbility_Stun::OnStunTimeEnded);
	WaitDelayTask->ReadyForActivation();
	
	CommitAbility(Handle, ActorInfo, ActivationInfo);
}

void USMGameplayAbility_Stun::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	OnStunEnded();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USMGameplayAbility_Stun::OnStunTimeEnded()
{
	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensure(SourceASC))
	{
		return;
	}

	// 잡을 수 없는 태그를 부착합니다. 추가 잡기를 방지합니다.
	SourceASC->AddTag(SMTags::Character::State::Uncatchable);

	// 스매시 당하는 상태인 경우의 처리입니다.
	if (SourceASC->HasMatchingGameplayTag(SMTags::Character::State::Smashed))
	{
		ProcessBuzzerBeaterSmashed();
		return;
	}
	// 잡힌 상태인 경우의 처리입니다.
	else if (SourceASC->HasMatchingGameplayTag(SMTags::Character::State::Caught))
	{
		ProcessCaughtExit();
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
	NET_LOG(GetSMPlayerCharacterFromActorInfo(), Log, TEXT("버저 비터 상태 진입"));
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
	NET_LOG(GetSMPlayerCharacterFromActorInfo(), Log, TEXT("버저 비터 종료"));
	UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("StandUp"), SmashedMontage, 1.0f, TEXT("End"));
	if (ensure(PlayMontageAndWaitTask))
	{
		ClientRPCPlayMontage(SmashedMontage, 1.0f, TEXT("End"));
		PlayMontageAndWaitTask->OnBlendOut.AddDynamic(this, &USMGameplayAbility_Stun::OnComplete);
		PlayMontageAndWaitTask->ReadyForActivation();
	}
}

void USMGameplayAbility_Stun::ProcessCaughtExit()
{
	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensure(SourceASC))
	{
		return;
	}

	// 잡기 탈출 GA를 활성화합니다.
	SourceASC->TryActivateAbilitiesByTag(FGameplayTagContainer(SMTags::Ability::CaughtExitOnStunEnd));

	UAbilityTask_WaitGameplayEvent* WatiGameplayEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::Character::CaughtExitEnd);
	if (ensure(WatiGameplayEvent))
	{
		WatiGameplayEvent->EventReceived.AddDynamic(this, &USMGameplayAbility_Stun::OnCaughtExitEnded);
		WatiGameplayEvent->ReadyForActivation();
	}
}

void USMGameplayAbility_Stun::OnCaughtExitEnded(FGameplayEventData Payload)
{
	OnComplete();
}

void USMGameplayAbility_Stun::ResetStunState()
{
	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensure(SourceASC))
	{
		return;
	}

	// 매쳐졌는지 아닌지 확인하고 그에 맞는 스턴 종료 애니메이션을 재생시킵니다.
	UAnimMontage* EndMontage = StunMontage;
	UAnimInstance* SourceAnimInstace = CurrentActorInfo->GetAnimInstance();
	if (ensure(SourceAnimInstace))
	{
		FAnimMontageInstance* MontageInstance = SourceAnimInstace->GetActiveMontageInstance();
		if (MontageInstance)
		{
			if (MontageInstance->Montage == SmashedMontage)
			{
				EndMontage = SmashedMontage;
			}
		}
	}
	
	UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("StunEnd"), EndMontage, 1.0f, TEXT("End"));
	if (ensure(PlayMontageAndWaitTask))
	{
		ClientRPCPlayMontage(EndMontage, 1.0f, TEXT("End"));
		PlayMontageAndWaitTask->OnBlendOut.AddDynamic(this, &USMGameplayAbility_Stun::OnComplete);
		PlayMontageAndWaitTask->ReadyForActivation();
	}
}

void USMGameplayAbility_Stun::OnComplete()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USMGameplayAbility_Stun::OnStunEnded()
{
	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensure(SourceASC))
	{
		return;
	}

	if (CurrentActorInfo->IsNetAuthority())
	{
		// 컨트롤 로테이션을 따라가도록 복구해줍니다.
		ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
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
