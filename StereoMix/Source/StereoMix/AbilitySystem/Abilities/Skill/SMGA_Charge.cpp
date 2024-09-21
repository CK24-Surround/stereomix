// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_Charge.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "AbilitySystem/Task/SMAT_AdjustableDash.h"
#include "AbilitySystem/Task/SMAT_WaitChargeBlocked.h"
#include "Characters/Player/SMBassCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Data/Character/SMBassCharacterDataAsset.h"
#include "Data/DataTable/SMCharacterData.h"
#include "FunctionLibraries/SMDataTableFunctionLibrary.h"
#include "Utilities/SMCollision.h"

USMGA_Charge::USMGA_Charge()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

	ActivationOwnedTags.AddTag(SMTags::Character::State::Charge);

	if (FSMCharacterSkillData* SkillData = USMDataTableFunctionLibrary::GetCharacterSkillData(ESMCharacterType::Bass))
	{
		Damage = SkillData->Damage;
		StunTime = SkillData->Duration;
	}
}

void USMGA_Charge::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMBassCharacter* SourceCharacter = GetAvatarActor<ASMBassCharacter>();
	USMAbilitySystemComponent* SourceASC = GetASC<USMAbilitySystemComponent>();
	UCapsuleComponent* SourceCapsule = SourceCharacter ? SourceCharacter->GetCapsuleComponent() : nullptr;
	UBoxComponent* SourceChargeCollider = SourceCharacter ? SourceCharacter->GetChargeColliderComponent() : nullptr;
	const USMBassCharacterDataAsset* SourceDataAsset = SourceCharacter ? SourceCharacter->GetDataAsset<USMBassCharacterDataAsset>() : nullptr;
	if (!SourceCharacter || !SourceASC || !SourceCapsule || !SourceChargeCollider || !SourceDataAsset)
	{
		EndAbilityByCancel();
		return;
	}

	K2_CommitAbility();

	const FName TaskName = TEXT("ChargeMontage");
	UAnimMontage* ChargeMontage = SourceDataAsset->SkillMontage[SourceCharacter->GetTeam()];
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TaskName, ChargeMontage);
	MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnChargeEnded);
	MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnChargeEnded);
	MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnChargeEnded);
	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnChargeEnded);
	MontageTask->ReadyForActivation();

	OriginalCapsuleCollisionProfileName = SourceCapsule->GetCollisionProfileName();
	SourceCapsule->SetCollisionProfileName(SMCollisionProfileName::Ghost);

	OriginalChargeCollisionProfileName = SourceChargeCollider->GetCollisionProfileName();
	SourceChargeCollider->SetCollisionProfileName(SMCollisionProfileName::Charge);

	if (IsLocallyControlled())
	{
		USMAT_WaitChargeBlocked* ChargeBlockedTask = USMAT_WaitChargeBlocked::WaitChargeBlocked(this);
		ChargeBlockedTask->OnChargeBlocked.BindUObject(this, &ThisClass::OnChargeBlocked);
		ChargeBlockedTask->ReadyForActivation();

		// 돌진하며 방향 전환을 가능하게 합니다.
		USMAT_AdjustableDash* AdjustableDashTask = USMAT_AdjustableDash::AdjustableDash(this, RotationPerSecond);
		AdjustableDashTask->ReadyForActivation();

		// 박지 않고 끝나더라도 이펙트를 종료할 수 있도록 이벤트를 받습니다.
		UAbilityTask_WaitGameplayEvent* ChargeEndEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::AnimNotify::ChargeEndEntry);
		ChargeEndEventTask->EventReceived.AddDynamic(this, &ThisClass::OnChargeEndEntry);
		ChargeEndEventTask->ReadyForActivation();

		FGameplayCueParameters GCParams;
		SourceASC->AddGC(SourceCharacter, SMTags::GameplayCue::Bass::Charge, GCParams);

		SourceCharacter->FocusToCursor();
	}
}

void USMGA_Charge::OnChargeBlocked(AActor* TargetActor)
{
	USMAbilitySystemComponent* SourceASC = GetASC<USMAbilitySystemComponent>();
	if (!SourceASC)
	{
		return;
	}

	const FName SectionName = TEXT("End");
	MontageJumpToSection(SectionName);

	NET_LOG(GetAvatarActor(), Log, TEXT("돌진에 적중된 대상: %s"), *GetNameSafe(TargetActor));
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetASC)
	{
		ServerRequestEffect(TargetActor);
	}

	FGameplayCueParameters ChargeGCParams;
	SourceASC->RemoveGC(GetAvatarActor(), SMTags::GameplayCue::Bass::Charge, ChargeGCParams);

	FGameplayCueParameters ChargeHitGCParams;
	ChargeHitGCParams.Instigator = GetAvatarActor();
	SourceASC->ExecuteGC(TargetActor, SMTags::GameplayCue::Bass::ChargeHit, ChargeHitGCParams);
}

void USMGA_Charge::OnChargeEndEntry(FGameplayEventData Payload)
{
	USMAbilitySystemComponent* SourceASC = GetASC<USMAbilitySystemComponent>();
	if (!SourceASC)
	{
		return;
	}

	FGameplayCueParameters GCParams;
	SourceASC->RemoveGC(GetAvatarActor(), SMTags::GameplayCue::Bass::Charge, GCParams);
}

void USMGA_Charge::OnChargeEnded()
{
	ASMBassCharacter* SourceCharacter = GetAvatarActor<ASMBassCharacter>();
	UCapsuleComponent* SourceCapsule = SourceCharacter->GetCapsuleComponent();
	UBoxComponent* SourceChargeCollider = SourceCharacter->GetChargeColliderComponent();
	if (!SourceCharacter || !SourceCapsule || !SourceChargeCollider)
	{
		EndAbilityByCancel();
		return;
	}

	SourceCapsule->SetCollisionProfileName(OriginalCapsuleCollisionProfileName);
	SourceChargeCollider->SetCollisionProfileName(OriginalChargeCollisionProfileName);

	K2_EndAbilityLocally();
}

void USMGA_Charge::ServerRequestEffect_Implementation(AActor* TargetActor)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	const USMPlayerCharacterDataAsset* SourceDataAsset = GetDataAsset();
	if (!TargetASC || !SourceDataAsset)
	{
		return;
	}

	FGameplayEventData EventData;
	EventData.EventMagnitude = StunTime;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetActor, SMTags::Event::Character::Stun, EventData);

	FGameplayEffectSpecHandle GESpecHandle = MakeOutgoingGameplayEffectSpec(SourceDataAsset->DamageGE);
	if (GESpecHandle.IsValid())
	{
		GESpecHandle.Data->SetSetByCallerMagnitude(SMTags::Data::Damage, Damage);
		TargetASC->BP_ApplyGameplayEffectSpecToSelf(GESpecHandle);
	}
}
