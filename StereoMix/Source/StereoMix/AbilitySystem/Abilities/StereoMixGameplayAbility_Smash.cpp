// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixGameplayAbility_Smash.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/StereoMixAbilitySystemComponent.h"
#include "Characters/StereoMixPlayerCharacter.h"
#include "Utilities/StereoMixTag.h"

UStereoMixGameplayAbility_Smash::UStereoMixGameplayAbility_Smash()
{
	ActivationOwnedTags = FGameplayTagContainer(StereoMixTag::Character::State::Smashing);
	ActivationRequiredTags = FGameplayTagContainer(StereoMixTag::Character::State::Catch);
}
// TODO: Smash는 그냥 Smash에서 디태치 로직 실행하도록 수정 예정
void UStereoMixGameplayAbility_Smash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UStereoMixAbilitySystemComponent* SourceASC = GetStereoMixAbilitySystemComponentFromActorInfo();
	if (ensure(SourceASC))
	{
		AStereoMixPlayerCharacter* TargetCharacter = Cast<AStereoMixPlayerCharacter>(SourceASC->GetCurrentCatchPawn());
		UStereoMixAbilitySystemComponent* TargetASC = Cast<UStereoMixAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetCharacter));
		if (ensure(TargetCharacter && TargetASC))
		{
			UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, StereoMixTag::Event::AnimNotify::Smash);
			if (ensure(WaitGameplayEventTask))
			{
				WaitGameplayEventTask->EventReceived.AddDynamic(this, &UStereoMixGameplayAbility_Smash::OnSmash);
				WaitGameplayEventTask->ReadyForActivation();
			}

			UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("SmashMontage"), SmashMontage);
			if (ensure(PlayMontageAndWaitTask))
			{
				CommitAbility(Handle, ActorInfo, ActivationInfo);
				
				// 타겟 측에 스매시 당하는 중을 표시하는 태그를 부착합니다.
				if (ActorInfo->IsNetAuthority())
				{
					TargetASC->AddLooseGameplayTag(StereoMixTag::Character::State::Smashed);
					TargetASC->AddReplicatedLooseGameplayTag(StereoMixTag::Character::State::Smashed);
				}

				PlayMontageAndWaitTask->OnBlendOut.AddDynamic(this, &UStereoMixGameplayAbility_Smash::OnCompleted);
				PlayMontageAndWaitTask->ReadyForActivation();

				return;
			}
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}

void UStereoMixGameplayAbility_Smash::OnCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UStereoMixGameplayAbility_Smash::OnSmash(FGameplayEventData Payload)
{
	UStereoMixAbilitySystemComponent* SourceASC = GetStereoMixAbilitySystemComponentFromActorInfo();
	if (ensure(SourceASC))
	{
		AStereoMixPlayerCharacter* TargetCharacter = Cast<AStereoMixPlayerCharacter>(SourceASC->GetCurrentCatchPawn());
		UStereoMixAbilitySystemComponent* TargetASC = Cast<UStereoMixAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetCharacter));
		if (ensure(TargetCharacter && TargetASC))
		{
			if (CurrentActorInfo->IsNetAuthority())
			{
				if (TargetASC->HasMatchingGameplayTag(StereoMixTag::Character::State::Smashed))
				{
					TargetASC->RemoveLooseGameplayTag(StereoMixTag::Character::State::Smashed);
					TargetASC->RemoveReplicatedLooseGameplayTag(StereoMixTag::Character::State::Smashed);
				}
			}

			// TODO: 타일 트리거 로직

			// 스매시 공격 이벤트를 타겟에게 보냅니다. 여기서 디태치 등 여러 작업을 수행합니다.
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetCharacter, StereoMixTag::Event::Character::OnSmash, FGameplayEventData());
			
			// 타겟의 Smashed 어빌리티를 활성화합니다.
			TargetASC->TryActivateAbilitiesByTag(FGameplayTagContainer(StereoMixTag::Ability::Smashed));
		}
	}
}
