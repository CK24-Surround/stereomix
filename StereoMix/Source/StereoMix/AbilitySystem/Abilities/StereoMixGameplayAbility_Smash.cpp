// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixGameplayAbility_Smash.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Characters/StereoMixPlayerCharacter.h"
#include "Utilities/StereoMixLog.h"
#include "Utilities/StereoMixTag.h"

UStereoMixGameplayAbility_Smash::UStereoMixGameplayAbility_Smash()
{
	ActivationOwnedTags = FGameplayTagContainer(StereoMixTag::Character::State::Smashing);
	ActivationRequiredTags = FGameplayTagContainer(StereoMixTag::Character::State::Catch);

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
}

void UStereoMixGameplayAbility_Smash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!ensure(ActorInfo))
	{
		return;
	}

	const AStereoMixPlayerCharacter* StereoMixCharacter = Cast<AStereoMixPlayerCharacter>(ActorInfo->AvatarActor.Get());
	if (!StereoMixCharacter)
	{
		NET_LOG(ActorInfo->AvatarActor.Get(), Warning, TEXT("StereoMixCharacter로 캐스팅 실패했습니다."));
		return;
	}

	UAbilitySystemComponent* SourceASC = ActorInfo->AbilitySystemComponent.Get();
	if (!SourceASC)
	{
		NET_LOG(ActorInfo->AvatarActor.Get(), Warning, TEXT("ASC가 유효하지 않습니다."));
		return;
	}

	TArray<AActor*> AttachedActors;
	StereoMixCharacter->GetAttachedActors(AttachedActors);

	if (AttachedActors.IsEmpty())
	{
		NET_LOG(CurrentActorInfo->AvatarActor.Get(), Warning, TEXT("잡힌 대상이 없습니다."));
		return;
	}

	AStereoMixPlayerCharacter* TargetStereoMixCharacter = Cast<AStereoMixPlayerCharacter>(AttachedActors[0]);
	if (!TargetStereoMixCharacter)
	{
		NET_LOG(CurrentActorInfo->AvatarActor.Get(), Warning, TEXT("잡힌 대상을 AStereoMixPlayerCharacter로 캐스팅하는데 실패했습니다."));
		return;
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetStereoMixCharacter);
	if (!TargetASC)
	{
		NET_LOG(CurrentActorInfo->AvatarActor.Get(), Warning, TEXT("잡힌 대상의 ASC가 유효하지 않습니다."));
		return;
	}

	UAbilityTask_PlayMontageAndWait* AbilityTaskPlayMontageAndWait = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("SmashMontage"), SmashMontage);
	if (AbilityTaskPlayMontageAndWait)
	{
		// 아마 스매시는 다른 상호작용을 무시하기 때문에 OnCompleted로 충분할 것입니다.
		AbilityTaskPlayMontageAndWait->OnCompleted.AddDynamic(this, &UStereoMixGameplayAbility_Smash::OnCompleted);
		AbilityTaskPlayMontageAndWait->ReadyForActivation();
		if (AddSmashedState)
		{
			FGameplayEffectContextHandle GEContext = SourceASC->MakeEffectContext();
			if (GEContext.IsValid())
			{
				SourceASC->BP_ApplyGameplayEffectToTarget(AddSmashedState, TargetASC, 1.0f, GEContext);
			}
		}
	}

	UAbilityTask_WaitGameplayEvent* AbilityTask_WaitGameplayEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, StereoMixTag::Event::Character::Smash);
	if (AbilityTask_WaitGameplayEvent)
	{
		AbilityTask_WaitGameplayEvent->EventReceived.AddDynamic(this, &UStereoMixGameplayAbility_Smash::OnEventReceived);
		AbilityTask_WaitGameplayEvent->ReadyForActivation();
	}
}

void UStereoMixGameplayAbility_Smash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UStereoMixGameplayAbility_Smash::OnCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UStereoMixGameplayAbility_Smash::OnEventReceived(FGameplayEventData Payload)
{
	if (!ensure(CurrentActorInfo))
	{
		return;
	}

	AStereoMixPlayerCharacter* StereoMixCharacter = Cast<AStereoMixPlayerCharacter>(CurrentActorInfo->AvatarActor.Get());
	if (!StereoMixCharacter)
	{
		NET_LOG(CurrentActorInfo->AvatarActor.Get(), Warning, TEXT("아바타의 캐스팅에 실패했습니다."));
		return;
	}

	UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(StereoMixCharacter);
	if (!SourceASC)
	{
		NET_LOG(CurrentActorInfo->AvatarActor.Get(), Warning, TEXT("아바타의 ASC가 유효하지 않습니다."));
		return;
	}

	TArray<AActor*> AttachedActors;
	StereoMixCharacter->GetAttachedActors(AttachedActors);

	if (AttachedActors.IsEmpty())
	{
		NET_LOG(CurrentActorInfo->AvatarActor.Get(), Warning, TEXT("잡힌 대상이 없습니다."));
		return;
	}

	AStereoMixPlayerCharacter* TargetStereoMixCharacter = Cast<AStereoMixPlayerCharacter>(AttachedActors[0]);
	if (!TargetStereoMixCharacter)
	{
		NET_LOG(CurrentActorInfo->AvatarActor.Get(), Warning, TEXT("잡힌 대상을 AStereoMixPlayerCharacter로 캐스팅하는데 실패했습니다."));
		return;
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetStereoMixCharacter);
	if (!TargetASC)
	{
		NET_LOG(CurrentActorInfo->AvatarActor.Get(), Warning, TEXT("잡힌 대상의 ASC가 유효하지 않습니다."));
		return;
	}

	FGameplayEffectContextHandle GEContextHandle = SourceASC->MakeEffectContext();
	if (GEContextHandle.IsValid())
	{
		SourceASC->BP_ApplyGameplayEffectToTarget(RemoveSmashedState, TargetASC, 1.0f, GEContextHandle);
	}

	FGameplayEventData PayloadData;
	PayloadData.Instigator = StereoMixCharacter;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetStereoMixCharacter, StereoMixTag::Event::Character::Smashed, PayloadData);
}
