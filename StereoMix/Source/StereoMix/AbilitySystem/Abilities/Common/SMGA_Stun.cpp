// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_Stun.h"

#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "Data/DataAsset/Character/SMPlayerCharacterDataAsset.h"


USMGA_Stun::USMGA_Stun()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	ActivationOwnedTags.AddTag(SMTags::Character::State::Common::Stun);

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = SMTags::Event::Character::Stun;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void USMGA_Stun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	USMAbilitySystemComponent* SourceASC = GetASC();
	const USMPlayerCharacterDataAsset* SourceDataAsset = GetDataAsset();
	if (!SourceCharacter || !SourceASC || !SourceDataAsset || !TriggerEventData)
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

	if (K2_HasAuthority())
	{
		Instigator = const_cast<AActor*>(TriggerEventData->Instigator.Get());

		const UCapsuleComponent* CapsuleComponent = SourceCharacter ? SourceCharacter->GetCapsuleComponent() : nullptr;
		const float CapsuleHalfHeight = CapsuleComponent ? CapsuleComponent->GetScaledCapsuleHalfHeight() : 0.0f;
		const FVector Offset(0.0, 0.0, CapsuleHalfHeight -30.0);

		FGameplayCueParameters GCParams;
		GCParams.SourceObject = SourceCharacter;
		GCParams.TargetAttachComponent = SourceCharacter->GetRootComponent();
		GCParams.Location = Offset;
		SourceASC->AddGC(Instigator, SMTags::GameplayCue::Common::Stun, GCParams);
	}
}

void USMGA_Stun::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (K2_HasAuthority())
	{
		if (USMAbilitySystemComponent* SourceASC = GetASC())
		{
			AActor* SourceActor = GetAvatarActor();
			FGameplayCueParameters GCParams;
			GCParams.SourceObject = SourceActor;
			SourceASC->RemoveGC(Instigator, SMTags::GameplayCue::Common::Stun, GCParams);
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USMGA_Stun::OnStunEnded()
{
	UAbilityTask_NetworkSyncPoint* SyncTask = UAbilityTask_NetworkSyncPoint::WaitNetSync(this, EAbilityTaskNetSyncType::BothWait);
	SyncTask->OnSync.AddDynamic(this, &ThisClass::K2_EndAbility);
	SyncTask->ReadyForActivation();
}
