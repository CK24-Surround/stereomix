// Fill out your copyright notice in the Description page of Project Settings.


#include "SMGameplayAbility_Smash.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Characters/SMPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Utilities/SMLog.h"
#include "Utilities/SMTagName.h"

USMGameplayAbility_Smash::USMGameplayAbility_Smash()
{
	SmashNotifyEventTag = FGameplayTag::RequestGameplayTag(SMTagName::Event::AnimNotify::Smash);
	SmashedStateTag = FGameplayTag::RequestGameplayTag(SMTagName::Character::State::Smashed);
	SmashedAbilityTag = FGameplayTag::RequestGameplayTag(SMTagName::Ability::Smashed);
	CatchStateTag = FGameplayTag::RequestGameplayTag(SMTagName::Character::State::Catch);
	CaughtStateTag = FGameplayTag::RequestGameplayTag(SMTagName::Character::State::Caught);
	
	ActivationOwnedTags = FGameplayTagContainer(FGameplayTag::RequestGameplayTag(SMTagName::Character::State::Smashing));
	ActivationRequiredTags = FGameplayTagContainer(FGameplayTag::RequestGameplayTag(SMTagName::Character::State::Catch));
}

void USMGameplayAbility_Smash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	USMAbilitySystemComponent* SourceASC = GetStereoMixAbilitySystemComponentFromActorInfo();
	if (!ensure(SourceASC))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ASMPlayerCharacter* SourceCharacter = GetStereoMixPlayerCharacterFromActorInfo();
	if (!ensure(SourceCharacter))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ASMPlayerCharacter* TargetCharacter = SourceCharacter->GetCatchCharacter();
	if (!ensure(TargetCharacter))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	USMAbilitySystemComponent* TargetASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetCharacter));
	if (!ensure(TargetASC))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 애님 노티파이의 이벤트를 기다립니다.
	UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SmashNotifyEventTag);
	if (!ensure(WaitGameplayEventTask))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	WaitGameplayEventTask->EventReceived.AddDynamic(this, &USMGameplayAbility_Smash::OnSmash);
	WaitGameplayEventTask->ReadyForActivation();

	// 스매시 애니메이션을 재생합니다.
	UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("SmashMontage"), SmashMontage);
	if (!ensure(PlayMontageAndWaitTask))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	PlayMontageAndWaitTask->OnBlendOut.AddDynamic(this, &USMGameplayAbility_Smash::OnCompleted);
	PlayMontageAndWaitTask->ReadyForActivation();

	// 타겟 측에 스매시 당하는 중을 표시하는 태그를 부착합니다.
	if (ActorInfo->IsNetAuthority())
	{
		TargetASC->AddTag(SmashedStateTag);
	}

	CommitAbility(Handle, ActorInfo, ActivationInfo);
}

void USMGameplayAbility_Smash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	NET_LOG(GetAvatarActorFromActorInfo(), Warning, TEXT("EndAbility 호출"));
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USMGameplayAbility_Smash::OnCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

// TODO: 종종 서버에서 호출되지 않는 오류가 종종 발생함.
void USMGameplayAbility_Smash::OnSmash(FGameplayEventData Payload)
{
	NET_LOG(GetAvatarActorFromActorInfo(), Warning, TEXT("OnSmash 호출"));
	ASMPlayerCharacter* SourceCharacter = GetStereoMixPlayerCharacterFromActorInfo();
	if (!ensure(SourceCharacter))
	{
		return;
	}

	ASMPlayerCharacter* TargetCharacter = SourceCharacter->GetCatchCharacter();
	if (!ensure(TargetCharacter))
	{
		return;
	}

	USMAbilitySystemComponent* TargetASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetCharacter));
	if (!ensure(TargetASC))
	{
		return;
	}

	if (CurrentActorInfo->IsNetAuthority())
	{
		ReleaseCatch(TargetCharacter);

		// 스매시 당하는 상태를 나타내는 태그를 제거해줍니다.
		TargetASC->RemoveTag(SmashedStateTag);
	}

	// TODO: 타일 트리거 로직

	// 타겟의 Smashed 어빌리티를 활성화합니다.
	TargetASC->TryActivateAbilitiesByTag(FGameplayTagContainer(SmashedAbilityTag));
}

void USMGameplayAbility_Smash::ReleaseCatch(ASMPlayerCharacter* TargetCharacter)
{
	if (!ensure(TargetCharacter))
	{
		return;
	}

	USMAbilitySystemComponent* TargetASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetCharacter));
	if (!ensure(TargetASC))
	{
		return;
	}

	ASMPlayerCharacter* SourceCharacter = GetStereoMixPlayerCharacterFromActorInfo();
	if (!ensure(SourceCharacter))
	{
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetStereoMixAbilitySystemComponentFromActorInfo();
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
	SourceASC->RemoveTag(CatchStateTag);
	TargetASC->RemoveTag(CaughtStateTag);

	SourceCharacter->SetCatchCharacter(nullptr);
	TargetCharacter->SetCaughtCharacter(nullptr);
}
