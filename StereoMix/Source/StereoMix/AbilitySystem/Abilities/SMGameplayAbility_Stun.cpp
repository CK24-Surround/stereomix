// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayAbility_Stun.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Characters/SMPlayerCharacter.h"
#include "Utilities/SMLog.h"
#include "AbilitySystem/SMTags.h"
#include "Components/SMCatchInteractionComponent_Character.h"
#include "FunctionLibraries/SMCatchInteractionBlueprintLibrary.h"

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

	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensureAlways(SourceASC))
	{
		EndAbilityByCancel();
		return;
	}

	USMCatchInteractionComponent_Character* SourceCIC = Cast<USMCatchInteractionComponent_Character>(SourceCharacter->GetCatchInteractionComponent());
	if (!ensureAlways(SourceCIC))
	{
		EndAbilityByCancel();
		return;
	}

	ESMTeam SourceTeam = SourceCharacter->GetTeam();
	CachedStunMontage = StunMontage.FindOrAdd(SourceTeam, nullptr);
	CachedSmashedMontage = SmashedMontage.FindOrAdd(SourceTeam, nullptr);
	CachedCatchExitMontage = CatchExitMontage.FindOrAdd(SourceTeam, nullptr);

	// 스턴 몽타주가 정상적으로 실행되면 0.0f 이상의 값을 반환합니다.
	const float Duration = SourceASC->PlayMontage(this, ActivationInfo, CachedStunMontage, 1.0f);
	ClientRPCPlayMontage(CachedStunMontage);
	if (!ensureAlways(Duration > 0.0f))
	{
		EndAbilityByCancel();
		return;
	}

	// 컨트롤 로테이션을 따라 회전하지 않도록 잠급니다.
	SourceCharacter->SetUseControllerRotation(false);

	// 스턴 시간 만큼 기다립니다.
	UAbilityTask_WaitDelay* WaitDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, StunTime);
	if (!ensureAlways(WaitDelayTask))
	{
		EndAbilityByCancel();
		return;
	}
	WaitDelayTask->OnFinish.AddDynamic(this, &ThisClass::OnStunTimeEnd);
	WaitDelayTask->ReadyForActivation();

	// 잡고 있던 액터가 있다면 놓아줍니다.
	if (SourceASC->HasMatchingGameplayTag(SMTags::Character::State::Catch))
	{
		AActor* TargetActor = SourceCIC->GetActorIAmCatching();
		USMCatchInteractionComponent* TargetCIC = USMCatchInteractionBlueprintLibrary::GetCatchInteractionComponent(TargetActor);
		if (ensureAlways(TargetCIC))
		{
			TargetCIC->OnCaughtReleased(SourceCharacter, false);
		}

		SourceCIC->SetActorIAmCatching(nullptr);
	}

	CommitAbility(Handle, ActorInfo, ActivationInfo);

	// 스턴 즉시 이펙트를 재생합니다.
	FGameplayCueParameters GCParams;
	GCParams.Instigator = SourceCharacter->GetLastAttackInstigator();
	GCParams.TargetAttachComponent = SourceCharacter->GetMesh();
	SourceASC->AddGameplayCue(SMTags::GameplayCue::Stun, GCParams);

	// 캐릭터 상태 위젯을 숨깁니다.
	SourceCharacter->SetCharacterStateVisibility(false);

	// 다른 클라이언트들에게 자신을 타겟하는 스크린 인디케이터를 활성화하도록 합니다.
	SourceCharacter->MulticastRPCAddScreenIndicatorToSelf(SourceCharacter);
}

void USMGameplayAbility_Stun::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!SourceCharacter)
	{
		Super::EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensureAlways(SourceASC))
	{
		Super::EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!bWasCancelled)
	{
		// 면역상태로 진입합니다. 딜레이로 인해 데미지를 받을 수도 있으니 GA실행 전에 먼저 면역태그를 추가합니다.
		SourceASC->AddTag(SMTags::Character::State::Immune);
		SourceASC->TryActivateAbilitiesByTag(FGameplayTagContainer(SMTags::Ability::Immune));
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USMGameplayAbility_Stun::OnStunTimeEnd()
{
	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensureAlways(SourceASC))
	{
		EndAbilityByCancel();
		return;
	}

	// 잡을 수 없는 태그를 부착합니다. 추가 잡기를 방지합니다.
	SourceASC->AddTag(SMTags::Character::State::Uncatchable);

	// 다른 클라이언트들에게 자신을 타겟하는 스크린 인디케이터를 제거하도록 합니다.
	SourceCharacter->MulticastRPCRemoveScreenIndicatorToSelf(SourceCharacter);

	// 스매시 당하는 상태인 경우의 처리입니다.
	if (SourceASC->HasMatchingGameplayTag(SMTags::Character::State::Smashed))
	{
		ProcessBuzzerBeaterSmashed();
	}
	// 잡힌 상태인 경우의 처리입니다.
	else if (SourceASC->HasMatchingGameplayTag(SMTags::Character::State::Caught))
	{
		ProcessCaughtExit();
	}
	// 기절 상태인 경우의 처리입니다.
	else
	{
		ResetStunState();
	}
}

void USMGameplayAbility_Stun::ProcessBuzzerBeaterSmashed()
{
	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensureAlways(SourceASC))
	{
		EndAbilityByCancel();
		return;
	}

	USMCatchInteractionComponent_Character* SourceCIC = Cast<USMCatchInteractionComponent_Character>(SourceCharacter->GetCatchInteractionComponent());
	if (!ensureAlways(SourceCIC))
	{
		EndAbilityByCancel();
		return;
	}

	// 만약 어떤 이유로 자신을 잡고 있는 캐릭터가 제거된 경우의 예외처리입니다.
	if (!SourceCIC->GetActorCatchingMe())
	{
		SourceASC->RemoveTag(SMTags::Character::State::Smashed);
		// 즉시 잡기 탈출 로직을 호출합니다.
		ProcessCaughtExit();
		return;
	}

	// 스매시 이벤트를 기다립니다.
	NET_LOG(GetSMPlayerCharacterFromActorInfo(), Log, TEXT("버저 비터 진입"));
	UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::Character::BuzzerBeaterSmashEnd);
	if (!ensureAlways(WaitGameplayEventTask))
	{
		EndAbilityByCancel();
		return;
	}
	WaitGameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::OnBuzzerBeaterSmashEnded);
	WaitGameplayEventTask->ReadyForActivation();
}

void USMGameplayAbility_Stun::OnBuzzerBeaterSmashEnded(FGameplayEventData Payload)
{
	// 버저비터 종료 애니메이션을 재생하고 이 애니메이션이 종료되면 스턴을 종료합니다. 이미 잡기는 풀린 상태입니다. 추가로 스턴 이펙트도 제거합니다.
	NET_LOG(GetSMPlayerCharacterFromActorInfo(), Log, TEXT("버저 비터 종료"));
	RemoveStunEffect();
	ClientRPCPlayMontage(CachedSmashedMontage, 1.0f, TEXT("End"));
	UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("StandUp"), CachedSmashedMontage, 1.0f, TEXT("End"));
	if (!ensureAlways(PlayMontageAndWaitTask))
	{
		EndAbilityByCancel();
		return;
	}
	PlayMontageAndWaitTask->OnCompleted.AddDynamic(this, &ThisClass::OnStunEnd);
	PlayMontageAndWaitTask->ReadyForActivation();
}

void USMGameplayAbility_Stun::ProcessCaughtExit()
{
	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!ensureAlways(SourceCharacter))
	{
		EndAbilityByCancel();
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensureAlways(SourceASC))
	{
		EndAbilityByCancel();
		return;
	}

	USMCatchInteractionComponent_Character* SourceCIC = Cast<USMCatchInteractionComponent_Character>(SourceCharacter->GetCatchInteractionComponent());
	if (!ensureAlways(SourceCIC))
	{
		EndAbilityByCancel();
		return;
	}

	// 잡히기 상태에서 탈출합니다. 추가로 스턴 이펙트도 제거합니다.
	SourceCIC->OnCaughtReleased(SourceCIC->GetActorCatchingMe(), true);
	RemoveStunEffect();

	// 잡기 탈출이 완료되기까지 기다립니다. 이후 스턴을 종료합니다.
	UAbilityTask_WaitGameplayEvent* WatiGameplayEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::Character::CaughtExitEnd);
	if (!ensureAlways(WatiGameplayEvent))
	{
		EndAbilityByCancel();
		return;
	}
	WatiGameplayEvent->EventReceived.AddDynamic(this, &ThisClass::OnCaughtExitEnd);
	WatiGameplayEvent->ReadyForActivation();
}

void USMGameplayAbility_Stun::OnCaughtExitEnd(FGameplayEventData Payload)
{
	OnStunEnd();
}

void USMGameplayAbility_Stun::ResetStunState()
{
	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!ensureAlways(SourceCharacter))
	{
		EndAbilityByCancel();
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensureAlways(SourceASC))
	{
		EndAbilityByCancel();
		return;
	}

	// 한번이라도 특수 액션을 당하면 캐릭터가 넘어진 상태가 됩니다. 이 상태에서는 스턴 종료 애니메이션 달라져야하는데, 이를 구분하기위한 코드입니다.
	// 특수 액션을 당하면 특수 액션 피격 애니메이션으로 무한루프 되고 있는 상태이므로 아래의 로직으로 구분할 수 있습니다.
	UAnimMontage* EndMontage = CachedStunMontage;
	UAnimInstance* SourceAnimInstace = CurrentActorInfo->GetAnimInstance();
	if (ensureAlways(SourceAnimInstace))
	{
		// 현재 재생중인 몽타주 인스턴스를 반환합니다.
		FAnimMontageInstance* MontageInstance = SourceAnimInstace->GetActiveMontageInstance();
		if (MontageInstance)
		{
			// 몽타주 인스턴스의 몽타주가 특수 액션의 몽타주와 동일한지 체크합니다.
			if (MontageInstance->Montage == CachedSmashedMontage)
			{
				EndMontage = CachedSmashedMontage;
			}
		}
	}

	// 위에서 구분된 몽타주로 스턴 종료 애니메이션을 재생합니다. 애니메이션 재생이 완료되면 스턴을 종료합니다. 추가로 스턴 이펙트도 제거합니다.
	RemoveStunEffect();
	ClientRPCPlayMontage(EndMontage, 1.0f, TEXT("End"));
	UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("StunEnd"), EndMontage, 1.0f, TEXT("End"));
	if (!ensureAlways(PlayMontageAndWaitTask))
	{
		EndAbilityByCancel();
		return;
	}
	PlayMontageAndWaitTask->OnCompleted.AddDynamic(this, &ThisClass::OnStunEnd);
	PlayMontageAndWaitTask->ReadyForActivation();

}

void USMGameplayAbility_Stun::OnStunEnd()
{
	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!ensureAlways(SourceCharacter))
	{
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensureAlways(SourceASC))
	{
		return;
	}

	USMCatchInteractionComponent_Character* SourceCIC = Cast<USMCatchInteractionComponent_Character>(SourceCharacter->GetCatchInteractionComponent());
	if (!ensureAlways(SourceCIC))
	{
		return;
	}

	// 컨트롤 로테이션을 따라가도록 복구해줍니다.
	SourceCharacter->SetUseControllerRotation(true);

	// 스턴이 완전히 종료되었기에 Uncatchable 태그를 제거하고 자신을 잡았던 캐릭터 리스트를 초기화합니다.
	SourceASC->RemoveTag(SMTags::Character::State::Uncatchable);
	SourceCIC->GetCapturedMeCharacters().Empty();

	// 스턴 종료 시 적용해야할 GE들을 적용합니다.
	for (const auto& StunEndedGE : StunEndedGEs)
	{
		if (ensure(StunEndedGE))
		{
			BP_ApplyGameplayEffectToOwner(StunEndedGE);
		}
	}

	// 캐릭터 상태 위젯을 다시 보이게합니다.
	SourceCharacter->SetCharacterStateVisibility(true);

	K2_EndAbility();
}

void USMGameplayAbility_Stun::RemoveStunEffect()
{
	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensureAlways(SourceASC))
	{
		return;
	}

	SourceASC->RemoveGameplayCue(SMTags::GameplayCue::Stun);
}