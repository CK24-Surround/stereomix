// Fill out your copyright notice in the Description page of Project Settings.


#include "StereoMixGameplayAbility_Smash.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/StereoMixAbilitySystemComponent.h"
#include "Characters/StereoMixPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
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
		AStereoMixPlayerCharacter* TargetCharacter = Cast<AStereoMixPlayerCharacter>(SourceASC->GetCurrentCatchCharacter());
		UStereoMixAbilitySystemComponent* TargetASC = Cast<UStereoMixAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetCharacter));
		if (ensure(TargetCharacter && TargetASC))
		{
			// 애님 노티파이의 이벤트를 기다립니다.
			UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, StereoMixTag::Event::AnimNotify::Smash);
			if (ensure(WaitGameplayEventTask))
			{
				WaitGameplayEventTask->EventReceived.AddDynamic(this, &UStereoMixGameplayAbility_Smash::OnSmash);
				WaitGameplayEventTask->ReadyForActivation();
			}

			// 스매시 애니메이션을 재생합니다.
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
		AStereoMixPlayerCharacter* TargetCharacter = Cast<AStereoMixPlayerCharacter>(SourceASC->GetCurrentCatchCharacter());
		UStereoMixAbilitySystemComponent* TargetASC = Cast<UStereoMixAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetCharacter));
		if (ensure(TargetCharacter && TargetASC))
		{
			// TODO: 태그 제거, 디태치 코드 필요
			if (CurrentActorInfo->IsNetAuthority())
			{
				// 스매시 당하는 상태를 나타내는 태그를 제거해줍니다.
				if (TargetASC->HasMatchingGameplayTag(StereoMixTag::Character::State::Smashed))
				{
					TargetASC->RemoveLooseGameplayTag(StereoMixTag::Character::State::Smashed);
					TargetASC->RemoveReplicatedLooseGameplayTag(StereoMixTag::Character::State::Smashed);
				}

				ReleaseCatch(TargetCharacter);
			}

			// TODO: 타일 트리거 로직

			// 스매시 공격 이벤트를 타겟에게 보냅니다. 여기서 디태치 등 여러 작업을 수행합니다.
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetCharacter, StereoMixTag::Event::Character::OnSmash, FGameplayEventData());

			// 타겟의 Smashed 어빌리티를 활성화합니다.
			TargetASC->TryActivateAbilitiesByTag(FGameplayTagContainer(StereoMixTag::Ability::Smashed));
		}
	}
}

void UStereoMixGameplayAbility_Smash::ReleaseCatch(AStereoMixPlayerCharacter* TargetCharacter)
{
	// TODO: 소스가 잡고있는 캐릭터를 디태치시켜야합니다.

	if (ensure(TargetCharacter))
	{
		UStereoMixAbilitySystemComponent* TargetASC = Cast<UStereoMixAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetCharacter));
		if (ensure(TargetASC))
		{
			// 디태치합니다.
			TargetCharacter->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

			// 콜리전, 움직임을 복구합니다.
			TargetCharacter->SetEnableCollision(true);
			TargetCharacter->SetEnableMovement(true);

			// 회전을 소스의 방향에 맞게 리셋해줍니다.
			const float TargetYaw = TargetCharacter->GetActorRotation().Yaw;
			MulticastRPCSetYawRotation(TargetCharacter, TargetYaw);

			// 다시 서버로부터 위치조정을 하도록 변경합니다.
			UCharacterMovementComponent* TargetMovement = TargetCharacter->GetCharacterMovement();
			if (ensure(TargetMovement))
			{
				TargetMovement->bIgnoreClientMovementErrorChecksAndCorrection = false;
			}

			// 카메라 뷰도 원래대로 되돌려줍니다.
			APlayerController* SourcePlayerController = Cast<APlayerController>(TargetCharacter->GetController());
			if (ensure(SourcePlayerController))
			{
				SourcePlayerController->SetViewTargetWithBlend(TargetCharacter, 1.0f, VTBlend_Cubic);
			}

			// 잡기, 잡히기 상태를 나타내는 태그를 제거해줍니다.
			UStereoMixAbilitySystemComponent* SourceASC = GetStereoMixAbilitySystemComponentFromActorInfo();
			if (ensure(SourceASC))
			{
				if (SourceASC->HasMatchingGameplayTag(StereoMixTag::Character::State::Catch))
				{
					SourceASC->SetCurrentCatchCharacter(nullptr);
					SourceASC->RemoveLooseGameplayTag(StereoMixTag::Character::State::Catch);
					SourceASC->RemoveReplicatedLooseGameplayTag(StereoMixTag::Character::State::Catch);
				}

				if (TargetASC->HasMatchingGameplayTag(StereoMixTag::Character::State::Caught))
				{
					TargetASC->SetCurrentCaughtCharacter(nullptr);
					TargetASC->RemoveLooseGameplayTag(StereoMixTag::Character::State::Caught);
					TargetASC->RemoveReplicatedLooseGameplayTag(StereoMixTag::Character::State::Caught);
				}
			}
		}
	}
}

void UStereoMixGameplayAbility_Smash::MulticastRPCSetYawRotation_Implementation(AStereoMixPlayerCharacter* TargetCharacter, float InYaw) {}
