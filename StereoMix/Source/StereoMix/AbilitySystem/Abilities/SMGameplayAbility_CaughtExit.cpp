// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayAbility_CaughtExit.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "SMGameplayAbility_Stun.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Characters/SMPlayerCharacter.h"
#include "AbilitySystem/SMTags.h"
#include "Components/SMCatchInteractionComponent_Character.h"
#include "Utilities/SMCatchInteractionBlueprintLibrary.h"

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
	if (!ensureAlways(SourceASC))
	{
		K2_CancelAbility();
		return;
	}

	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!ensureAlways(SourceCharacter))
	{
		K2_CancelAbility();
		return;
	}

	USMCatchInteractionComponent_Character* SourceCIC = Cast<USMCatchInteractionComponent_Character>(SourceCharacter->GetCatchInteractionComponent());
	if (!ensureAlways(SourceCIC))
	{
		K2_CancelAbility();
		return;
	}

	// 잡던 도중에 타겟이 사라지질 수도 있으므로(예시: 게임을 종료) 이에 대한 예외처리를 해줍니다.
	USMAbilitySystemComponent* TargetASC = nullptr;
	USMCatchInteractionComponent_Character* TargetCIC = nullptr;
	ASMPlayerCharacter* TargetCharacter = Cast<ASMPlayerCharacter>(SourceCIC->GetActorCatchingMe());
	if (TargetCharacter)
	{
		TargetASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetCharacter));
		if (!ensureAlways(TargetASC))
		{
			K2_CancelAbility();
			return;
		}

		TargetCIC = Cast<USMCatchInteractionComponent_Character>(USMCatchInteractionBlueprintLibrary::GetCatchInteractionComponent(TargetCharacter));
		if (!ensureAlways(TargetCIC))
		{
			K2_CancelAbility();
			return;
		}
	}

	// 잡기 상태에서 벗어납니다.
	if (SourceCIC->HandleCaughtReleased(TargetCharacter))
	{
		K2_CancelAbility();
		return;
	}

	// 타겟이 있다면 타겟에 필요한 처리를 해줍니다.
	if (TargetCharacter)
	{
		TargetASC->RemoveTag(SMTags::Character::State::Catch);
		TargetCIC->SetActorIAmCatching(nullptr);
	}

	UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("CaughtExit"), CaughtExitMontage, 1.0f);
	if (!ensureAlways(PlayMontageAndWaitTask))
	{
		K2_CancelAbility();
		return;
	}
	ClientRPCPlayMontage(CaughtExitMontage);
	PlayMontageAndWaitTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
	PlayMontageAndWaitTask->ReadyForActivation();
}
