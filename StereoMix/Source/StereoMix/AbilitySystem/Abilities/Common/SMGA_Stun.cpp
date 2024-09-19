// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_Stun.h"

#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"


USMGA_Stun::USMGA_Stun()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	ActivationOwnedTags.AddTag(SMTags::Character::State::Stun);

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = SMTags::Event::Character::Stun;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void USMGA_Stun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	const USMPlayerCharacterDataAsset* SourceDataAsset = GetDataAsset();
	if (!SourceCharacter || !SourceDataAsset || !TriggerEventData)
	{
		EndAbilityByCancel();
		return;
	}

	const FName MontageTaskName = TEXT("MontageTask");
	UAnimMontage* StunMontage = SourceDataAsset->StunMontage[SourceCharacter->GetTeam()];
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, MontageTaskName, StunMontage);
	MontageTask->ReadyForActivation();

	const float StunTime = TriggerEventData->EventMagnitude;
	UAbilityTask_WaitDelay* WaitTask = UAbilityTask_WaitDelay::WaitDelay(this, StunTime);
	WaitTask->OnFinish.AddDynamic(this, &ThisClass::OnStunEnded);
	WaitTask->ReadyForActivation();

	NET_LOG(GetAvatarActor(), Log, TEXT("%s: %f초 스턴에 빠집니다."), *GetNameSafe(SourceCharacter), StunTime);
}

void USMGA_Stun::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USMGA_Stun::OnStunEnded()
{
	UAbilityTask_NetworkSyncPoint* SyncTask = UAbilityTask_NetworkSyncPoint::WaitNetSync(this, EAbilityTaskNetSyncType::BothWait);
	SyncTask->OnSync.AddDynamic(this, &ThisClass::K2_EndAbility);
	SyncTask->ReadyForActivation();
}
