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

void UStereoMixGameplayAbility_Smash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UStereoMixAbilitySystemComponent* SourceASC = GetStereoMixAbilitySystemComponentFromActorInfo();
	if (!ensure(SourceASC))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AStereoMixPlayerCharacter* SourceCharacter = GetStereoMixPlayerCharacterFromActorInfo();
	if (!ensure(SourceCharacter))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AStereoMixPlayerCharacter* TargetCharacter = SourceCharacter->GetCatchCharacter();
	if (ensure(TargetCharacter))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UStereoMixAbilitySystemComponent* TargetASC = Cast<UStereoMixAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetCharacter));
	if (!ensure(TargetASC))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 애님 노티파이의 이벤트를 기다립니다.
	UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, StereoMixTag::Event::AnimNotify::Smash);
	if (!ensure(WaitGameplayEventTask))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	WaitGameplayEventTask->EventReceived.AddDynamic(this, &UStereoMixGameplayAbility_Smash::OnSmash);
	WaitGameplayEventTask->ReadyForActivation();

	// 스매시 애니메이션을 재생합니다.
	UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("SmashMontage"), SmashMontage);
	if (!ensure(PlayMontageAndWaitTask))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	PlayMontageAndWaitTask->OnBlendOut.AddDynamic(this, &UStereoMixGameplayAbility_Smash::OnCompleted);
	PlayMontageAndWaitTask->ReadyForActivation();

	// 타겟 측에 스매시 당하는 중을 표시하는 태그를 부착합니다.
	if (ActorInfo->IsNetAuthority())
	{
		TargetASC->AddTag(StereoMixTag::Character::State::Smashed);
	}

	CommitAbility(Handle, ActorInfo, ActivationInfo);
}

void UStereoMixGameplayAbility_Smash::OnCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UStereoMixGameplayAbility_Smash::OnSmash(FGameplayEventData Payload)
{
	AStereoMixPlayerCharacter* SourceCharacter = GetStereoMixPlayerCharacterFromActorInfo();
	if (!ensure(SourceCharacter))
	{
		return;
	}

	AStereoMixPlayerCharacter* TargetCharacter = SourceCharacter->GetCatchCharacter();
	if (!ensure(TargetCharacter))
	{
		return;
	}

	UStereoMixAbilitySystemComponent* TargetASC = Cast<UStereoMixAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetCharacter));
	if (!ensure(TargetASC))
	{
		return;
	}

	if (CurrentActorInfo->IsNetAuthority())
	{
		ReleaseCatch(TargetCharacter);

		// 스매시 당하는 상태를 나타내는 태그를 제거해줍니다.
		TargetASC->RemoveTag(StereoMixTag::Character::State::Smashed);
	}

	// TODO: 타일 트리거 로직

	// 스매시 공격 발생 이벤트를 타겟에게 보냅니다. 여기서 디태치 등 여러 작업을 수행합니다.
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetCharacter, StereoMixTag::Event::Character::OnSmash, FGameplayEventData());

	// 타겟의 Smashed 어빌리티를 활성화합니다.
	TargetASC->TryActivateAbilitiesByTag(FGameplayTagContainer(StereoMixTag::Ability::Smashed));
}

void UStereoMixGameplayAbility_Smash::ReleaseCatch(AStereoMixPlayerCharacter* TargetCharacter)
{
	if (!ensure(TargetCharacter))
	{
		return;
	}

	UStereoMixAbilitySystemComponent* TargetASC = Cast<UStereoMixAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetCharacter));
	if (!ensure(TargetASC))
	{
		return;
	}

	AStereoMixPlayerCharacter* SourceCharacter = GetStereoMixPlayerCharacterFromActorInfo();
	if (!ensure(SourceCharacter))
	{
		return;
	}

	UStereoMixAbilitySystemComponent* SourceASC = GetStereoMixAbilitySystemComponentFromActorInfo();
	if (!ensure(SourceASC))
	{
		return;
	}

	// 디태치합니다.
	TargetCharacter->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	// 콜리전, 움직임을 복구합니다.
	TargetCharacter->SetEnableCollision(true);
	TargetCharacter->SetEnableMovement(true);

	// 회전을 소스의 방향에 맞게 리셋해줍니다.
	const float SourceYaw = SourceCharacter->GetActorRotation().Yaw;
	TargetCharacter->MulticastRPCSetYawRotation(SourceYaw);

	// 다시 서버로부터 위치 보정을 수행하도록 변경합니다.
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
	SourceASC->RemoveTag(StereoMixTag::Character::State::Catch);
	TargetASC->RemoveTag(StereoMixTag::Character::State::Caught);

	SourceCharacter->SetCatchCharacter(nullptr);
	TargetCharacter->SetCaughtCharacter(nullptr);
}
