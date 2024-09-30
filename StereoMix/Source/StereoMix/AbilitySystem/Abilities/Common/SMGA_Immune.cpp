// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_Immune.h"

#include "GameFramework/Character.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "Characters/Component/SMCharacterMovementComponent.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Utilities/SMLog.h"

USMGA_Immune::USMGA_Immune()
{
	AbilityTags = FGameplayTagContainer(SMTags::Ability::Immune);
}

void USMGA_Immune::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	USMAbilitySystemComponent* SourceASC = GetASC();
	USMCharacterMovementComponent* SourceMovement = SourceCharacter ? SourceCharacter->GetCharacterMovement<USMCharacterMovementComponent>() : nullptr;
	if (!SourceCharacter || !SourceASC || !SourceMovement)
	{
		EndAbilityByCancel();
		return;
	}

	NET_LOG(GetAvatarActor(), Log, TEXT("%s의 면역 활성화"), *GetNameSafe(GetAvatarActor()));

	// 면역 시간만큼 기다립니다.
	UAbilityTask_WaitDelay* WaitdelayTask = UAbilityTask_WaitDelay::WaitDelay(this, ImmuneTime);
	WaitdelayTask->OnFinish.AddDynamic(this, &ThisClass::OnImmuneEnd);
	WaitdelayTask->ReadyForActivation();

	// 면역 시간동안 이동속도를 증가시킵니다.
	SourceMovement->AddMoveSpeedBuff(MoveSpeedMultiplier, ImmuneTime);

	FGameplayCueParameters GCParams;
	GCParams.SourceObject = SourceCharacter;
	SourceASC->AddGC(SourceCharacter, SMTags::GameplayCue::Common::Immune, GCParams);
}

void USMGA_Immune::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	NET_LOG(GetAvatarActor(), Log, TEXT("%s의 면역 해제"), *GetNameSafe(GetAvatarActor()));

	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	USMAbilitySystemComponent* SourceASC = GetASC();
	if (SourceCharacter && SourceASC)
	{
		// 서버의 경우 태그를 제거해줍니다.
		if (K2_HasAuthority())
		{
			SourceASC->RemoveTag(SMTags::Character::State::Immune);
		}

		FGameplayCueParameters GCParams;
		GCParams.SourceObject = SourceCharacter;
		SourceASC->RemoveGC(SourceCharacter, SMTags::GameplayCue::Common::Immune, GCParams);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USMGA_Immune::OnImmuneEnd()
{
	UAbilityTask_NetworkSyncPoint* SyncTask = UAbilityTask_NetworkSyncPoint::WaitNetSync(this, EAbilityTaskNetSyncType::BothWait);
	SyncTask->OnSync.AddDynamic(this, &ThisClass::K2_EndAbility);
	SyncTask->ReadyForActivation();
}
