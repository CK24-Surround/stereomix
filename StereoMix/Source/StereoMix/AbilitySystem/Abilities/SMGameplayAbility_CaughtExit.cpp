// Fill out your copyright notice in the Description page of Project Settings.


#include "SMGameplayAbility_CaughtExit.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "SMGameplayAbility_Stun.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Characters/SMPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Utilities/SMTags.h"

USMGameplayAbility_CaughtExit::USMGameplayAbility_CaughtExit()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

	AbilityTags = FGameplayTagContainer(SMTags::Ability::CaughtExit);
}

void USMGameplayAbility_CaughtExit::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensure(SourceASC))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!ensure(SourceCharacter))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ASMPlayerCharacter* TargetCharacter = SourceCharacter->GetCaughtCharacter();
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

	if (ActorInfo->IsNetAuthority())
	{
		// 디태치와 필요한 처리를 해줍니다.
		DetachFromTargetCharacter(TargetCharacter);

		// Source의 Caught 태그와 Target의 Catch 태그를 제거합니다.
		SourceASC->RemoveTag(SMTags::Character::State::Caught);
		TargetASC->RemoveTag(SMTags::Character::State::Catch);

		// 마지막으로 Catch 및 Caught 캐릭터를 초기화해줍니다.
		SourceCharacter->SetCaughtCharacter(nullptr);
		TargetCharacter->SetCatchCharacter(nullptr);
	}

	UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("CaughtExit"), CaughtExitMontage, 1.0f);
	if (!ensure(PlayMontageAndWaitTask))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ClientRPCPlayMontage(CaughtExitMontage);
	PlayMontageAndWaitTask->OnBlendOut.AddDynamic(this, &USMGameplayAbility_CaughtExit::OnCaughtExitEnded);
	PlayMontageAndWaitTask->OnInterrupted.AddDynamic(this, &USMGameplayAbility_CaughtExit::OnCaughtExitEnded);
	PlayMontageAndWaitTask->ReadyForActivation();
}

void USMGameplayAbility_CaughtExit::DetachFromTargetCharacter(ASMPlayerCharacter* InTargetCharacter)
{
	if (!ensure(InTargetCharacter))
	{
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensure(SourceASC))
	{
		return;
	}

	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
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

	// 위치보정을 활성화해줍니다.
	UCharacterMovementComponent* SourceMovement = SourceCharacter->GetCharacterMovement();
	if (ensure(SourceMovement))
	{
		SourceMovement->bIgnoreClientMovementErrorChecksAndCorrection = false;
	}

	// 카메라 뷰를 원래대로 복구합니다.
	APlayerController* SourcePlayerController = Cast<APlayerController>(SourceCharacter->GetController());
	if (ensure(SourcePlayerController))
	{
		SourcePlayerController->SetViewTargetWithBlend(SourceCharacter, 1.0f, VTBlend_Cubic);
	}
}

void USMGameplayAbility_CaughtExit::OnCaughtExitEnded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
