// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayAbility_CaughtExit.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "SMGameplayAbility_Stun.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Characters/SMPlayerCharacter.h"
#include "AbilitySystem/SMTags.h"

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
		K2_CancelAbility();
		return;
	}

	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!ensure(SourceCharacter))
	{
		K2_CancelAbility();
		return;
	}

	// 잡던 도중에 타겟이 사라지질 수도 있으므로(예시: 게임을 종료) 이에 대한 예외처리를 해줍니다.
	USMAbilitySystemComponent* TargetASC = nullptr;
	ASMPlayerCharacter* TargetCharacter = Cast<ASMPlayerCharacter>(SourceCharacter->GetActorCatchingMe());
	if (TargetCharacter)
	{
		TargetASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetCharacter));
	}

	// 디태치와 필요한 처리를 해줍니다.
	if (TargetCharacter)
	{
		USMCatchInteractionComponent_Character* TargetCIC = TargetCharacter->GetCatchInteractionComponent();
		if (ensureAlways(TargetCIC))
		{
			
		}
	}

	
	
	DetachFromTargetCharacter(TargetCharacter);

	// Source의 Caught 태그와 Target의 Catch 태그를 제거합니다.
	SourceASC->RemoveTag(SMTags::Character::State::Caught);
	if (TargetASC)
	{
		TargetASC->RemoveTag(SMTags::Character::State::Catch);
	}

	// 마지막으로 Catch 및 Caught 캐릭터를 초기화해줍니다.
	SourceCharacter->SetCaughtCharacter(nullptr);
	if (TargetCharacter)
	{
		TargetCharacter->SetCatchCharacter(nullptr);
	}

	UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("CaughtExit"), CaughtExitMontage, 1.0f);
	if (!ensure(PlayMontageAndWaitTask))
	{
		K2_CancelAbility();
		return;
	}

	ClientRPCPlayMontage(CaughtExitMontage);
	PlayMontageAndWaitTask->OnCompleted.AddDynamic(this, &USMGameplayAbility_CaughtExit::OnCaughtExitEnded);
	PlayMontageAndWaitTask->ReadyForActivation();
}

void USMGameplayAbility_CaughtExit::DetachFromTargetCharacter(ASMPlayerCharacter* InTargetCharacter)
{
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

	// SourceCharacter->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	// 콜리전, 움직임을 복구합니다.
	// SourceCharacter->SetEnableCollision(true);
	// SourceCharacter->SetEnableMovement(true);

	// 회전을 재지정합니다.
	// float NewYaw = 0.0f;
	// if (InTargetCharacter)
	// {
	// 	NewYaw = InTargetCharacter->GetActorRotation().Yaw;
	// }
	// SourceCharacter->MulticastRPCSetYawRotation(NewYaw);
	//
	// // TODO: 애니메이션 불일치에 따른 임시 위치 조정 코드입니다. 추후 제거되어야합니다.
	// SourceCharacter->ServerRPCPreventGroundEmbedding();

	// 위치보정을 활성화해줍니다.
	// UCharacterMovementComponent* SourceMovement = SourceCharacter->GetCharacterMovement();
	// if (ensure(SourceMovement))
	// {
	// 	SourceMovement->bIgnoreClientMovementErrorChecksAndCorrection = false;
	// }

	// 카메라 뷰를 원래대로 복구합니다.
	// APlayerController* SourcePlayerController = Cast<APlayerController>(SourceCharacter->GetController());
	// if (ensure(SourcePlayerController))
	// {
	// 	SourcePlayerController->SetViewTargetWithBlend(SourceCharacter, 1.0f, VTBlend_Cubic);
	// }

	// 캐릭터 상태 위젯을 다시 보이게합니다.
	// SourceCharacter->SetCharacterStateVisibility(true);
}

void USMGameplayAbility_CaughtExit::OnCaughtExitEnded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
