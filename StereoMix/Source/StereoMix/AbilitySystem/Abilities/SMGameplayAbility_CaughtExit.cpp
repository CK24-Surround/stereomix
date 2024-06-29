// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayAbility_CaughtExit.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Characters/SMPlayerCharacter.h"
#include "Components/SMCatchInteractionComponent_Character.h"
#include "SMGameplayAbility_Stun.h"

USMGameplayAbility_CaughtExit::USMGameplayAbility_CaughtExit()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

	AbilityTags = FGameplayTagContainer(SMTags::Ability::CaughtExit);
}

void USMGameplayAbility_CaughtExit::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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

	// 잡던 도중에 타겟이 사라지질 수도 있으므로(예시: 게임을 종료) 이에 대한 예외처리를 해줍니다.
	USMCatchInteractionComponent_Character* TargetCIC = nullptr;
	ASMPlayerCharacter* TargetCharacter = Cast<ASMPlayerCharacter>(SourceCIC->GetActorCatchingMe());
	if (TargetCharacter)
	{
		TargetCIC = Cast<USMCatchInteractionComponent_Character>(TargetCharacter->GetCatchInteractionComponent());
		if (!ensureAlways(TargetCIC))
		{
			EndAbilityByCancel();
			return;
		}
	}

	// 잡기 상태에서 벗어납니다.
	SourceCIC->CaughtReleased(TargetCharacter);

	// 타겟이 있다면 타겟에 필요한 처리를 해줍니다.
	if (TargetCharacter)
	{
		TargetCIC->SetActorIAmCatching(nullptr);
	}

	CachedCaughtExitMontage = CaughtExitMontage.FindOrAdd(SourceCharacter->GetTeam(), nullptr);

	ClientRPCPlayMontage(CachedCaughtExitMontage);
	UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("CaughtExit"), CachedCaughtExitMontage, 1.0f);
	if (!ensureAlways(PlayMontageAndWaitTask))
	{
		EndAbilityByCancel();
		return;
	}
	PlayMontageAndWaitTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
	PlayMontageAndWaitTask->ReadyForActivation();
}
