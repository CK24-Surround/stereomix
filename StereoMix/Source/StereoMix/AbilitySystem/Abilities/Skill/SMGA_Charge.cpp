// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_Charge.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "AbilitySystem/Task/SMAT_AdjustableDash.h"
#include "AbilitySystem/Task/SMAT_WaitChargeBlocked.h"
#include "Actors/Character/Player/SMBassCharacter.h"
#include "Components/BoxComponent.h"
#include "Data/DataAsset/Character/SMBassCharacterDataAsset.h"
#include "Data/DataTable/SMCharacterData.h"
#include "FunctionLibraries/SMAbilitySystemBlueprintLibrary.h"
#include "FunctionLibraries/SMDataTableFunctionLibrary.h"
#include "Utilities/SMCollision.h"

USMGA_Charge::USMGA_Charge()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

	ActivationOwnedTags.AddTag(SMTags::Character::State::Bass::Charge);
	ActivationOwnedTags.AddTag(SMTags::Character::State::Common::Invincible);

	if (const FSMCharacterSkillData* SkillData = USMDataTableFunctionLibrary::GetCharacterSkillData(ESMCharacterType::Bass))
	{
		Damage = SkillData->Damage;
		StunTime = SkillData->Duration;
	}
}

void USMGA_Charge::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMBassCharacter* SourceCharacter = GetCharacter<ASMBassCharacter>();
	const USMAbilitySystemComponent* SourceASC = GetASC();
	UBoxComponent* SourceChargeCollider = SourceCharacter ? SourceCharacter->GetChargeColliderComponent() : nullptr;
	const USMBassCharacterDataAsset* SourceDataAsset = SourceCharacter ? SourceCharacter->GetDataAsset<USMBassCharacterDataAsset>() : nullptr;
	if (!SourceCharacter || !SourceASC || !SourceChargeCollider || !SourceDataAsset)
	{
		EndAbilityByCancel();
		return;
	}

	K2_CommitAbility();

	const FName TaskName = TEXT("ChargeMontage");
	UAnimMontage* ChargeMontage = SourceDataAsset->SkillMontage[SourceCharacter->GetTeam()];
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TaskName, ChargeMontage, ChargeMoveSpeedMultiplier);
	MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnChargeEndedSyncPoint);
	MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnChargeEndedSyncPoint);
	MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnChargeEndedSyncPoint);
	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnChargeEndedSyncPoint);
	MontageTask->ReadyForActivation();

	OriginalChargeCollisionProfileName = SourceChargeCollider->GetCollisionProfileName();
	SourceChargeCollider->SetCollisionProfileName(SMCollisionProfileName::Charge);

	if (IsLocallyControlled())
	{
		USMAT_WaitChargeBlocked* ChargeBlockedTask = USMAT_WaitChargeBlocked::WaitChargeBlocked(this);
		ChargeBlockedTask->OnChargeBlocked.BindUObject(this, &ThisClass::OnChargeBlocked);
		ChargeBlockedTask->ReadyForActivation();

		// 돌진하며 방향 전환을 가능하게 합니다.
		USMAT_AdjustableDash* AdjustableDashTask = USMAT_AdjustableDash::AdjustableDash(this, RotationPerSecond * ChargeMoveSpeedMultiplier);
		AdjustableDashTask->ReadyForActivation();

		// 박지 않고 끝나더라도 이펙트를 종료할 수 있도록 이벤트를 받습니다.
		UAbilityTask_WaitGameplayEvent* ChargeEndEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::AnimNotify::Bass::ChargeFail);
		ChargeEndEventTask->EventReceived.AddDynamic(this, &ThisClass::OnChargeFailed);
		ChargeEndEventTask->ReadyForActivation();

		FGameplayCueParameters GCParams;
		GCParams.SourceObject = SourceCharacter;
		GCParams.TargetAttachComponent = SourceCharacter->GetRootComponent();
		SourceASC->AddGC(SourceCharacter, SMTags::GameplayCue::Bass::Charge, GCParams);

		SourceCharacter->FocusToCursor();
	}
}

void USMGA_Charge::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	const ASMBassCharacter* SourceCharacter = GetCharacter<ASMBassCharacter>();

	if (UBoxComponent* SourceChargeCollider = SourceCharacter ? SourceCharacter->GetChargeColliderComponent() : nullptr)
	{
		SourceChargeCollider->SetCollisionProfileName(OriginalChargeCollisionProfileName);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USMGA_Charge::OnChargeBlocked(AActor* TargetActor)
{
	ASMBassCharacter* SourceCharacter = GetCharacter<ASMBassCharacter>();
	const USMAbilitySystemComponent* SourceASC = GetASC();
	if (!SourceCharacter || !SourceASC)
	{
		return;
	}

	NET_LOG(GetAvatarActor(), Log, TEXT("돌진에 적중된 대상: %s"), *GetNameSafe(TargetActor));

	const FName SectionName = TEXT("End");
	MontageJumpToSection(SectionName);

	const ISMDamageInterface* TargetDamageInterface = Cast<ISMDamageInterface>(TargetActor);
	if (TargetDamageInterface && !TargetDamageInterface->CanIgnoreAttack())
	{
		ServerRequestEffect(TargetActor);

		APlayerController* PlayerController = SourceCharacter ? SourceCharacter->GetController<APlayerController>() : nullptr;
		const USMPlayerCharacterDataAsset* SourceDataAsset = SourceCharacter->GetDataAsset();
		if (PlayerController && SourceDataAsset)
		{
			PlayerController->ClientStartCameraShake(SourceDataAsset->SkillHitCameraShake);
		}

		FGameplayCueParameters ChargeHitGCParams;
		ChargeHitGCParams.SourceObject = SourceCharacter;
		ChargeHitGCParams.TargetAttachComponent = TargetActor->GetRootComponent();
		ChargeHitGCParams.Normal = SourceCharacter->GetActorRotation().Vector();
		SourceASC->ExecuteGC(SourceCharacter, SMTags::GameplayCue::Bass::ChargeHit, ChargeHitGCParams);
	}
	else // 벽인 경우입니다.
	{
		if (const UBoxComponent* ChargeCollider = SourceCharacter ? SourceCharacter->GetChargeColliderComponent() : nullptr)
		{
			FGameplayCueParameters ChargeHitGCParams;
			ChargeHitGCParams.SourceObject = SourceCharacter;
			ChargeHitGCParams.Location = ChargeCollider->GetComponentLocation();
			ChargeHitGCParams.Normal = ChargeCollider->GetComponentRotation().Vector();
			SourceASC->ExecuteGC(SourceCharacter, SMTags::GameplayCue::Bass::ChargeHit, ChargeHitGCParams);
		}
	}

	FGameplayCueParameters ChargeGCParams;
	ChargeGCParams.SourceObject = SourceCharacter;
	SourceASC->RemoveGC(SourceCharacter, SMTags::GameplayCue::Bass::Charge, ChargeGCParams);

	FGameplayCueParameters ChargeCrushGCParams;
	ChargeCrushGCParams.SourceObject = SourceCharacter;
	ChargeCrushGCParams.Location = SourceCharacter->GetActorLocation();
	SourceASC->ExecuteGC(SourceCharacter, SMTags::GameplayCue::Bass::ChargeCrush, ChargeCrushGCParams);
}


void USMGA_Charge::ServerRequestEffect_Implementation(AActor* TargetActor)
{
	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	if (!SourceCharacter)
	{
		return;
	}

	// 이미 태스크에서 한번 거르는 작업을 했지만 지연시간 등의 이유로 스턴에 걸리면 안되는 상태에 진입할 수 있습니다. 이를 다시한번 필터링합니다.
	ISMDamageInterface* TargetDamageInterface = Cast<ISMDamageInterface>(TargetActor);
	if (!TargetDamageInterface || TargetDamageInterface->CanIgnoreAttack())
	{
		return;
	}

	TargetDamageInterface->ReceiveDamage(SourceCharacter, Damage);

	const APawn* TargetPawn = Cast<APawn>(TargetActor);
	APlayerController* TargetPlayerController = TargetPawn ? TargetPawn->GetController<APlayerController>() : nullptr;
	const USMPlayerCharacterDataAsset* SourceDataAsset = SourceCharacter ? SourceCharacter->GetDataAsset() : nullptr;
	if (TargetPlayerController && SourceDataAsset)
	{
		TargetPlayerController->ClientStartCameraShake(SourceDataAsset->SkillHitCameraShake);
	}


	if (const USMAbilitySystemComponent* TargetASC = USMAbilitySystemBlueprintLibrary::GetSMAbilitySystemComponent(TargetActor); IsValid(TargetASC))
	{
		FGameplayEventData EventData;
		EventData.Instigator = SourceCharacter;
		EventData.EventMagnitude = StunTime;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetActor, SMTags::Event::Character::Stun, EventData);
	}

	OnSkillHit.Broadcast();
}

void USMGA_Charge::OnChargeFailed(FGameplayEventData Payload)
{
	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	const USMAbilitySystemComponent* SourceASC = GetASC();
	if (!SourceCharacter || !SourceASC)
	{
		return;
	}

	FGameplayCueParameters GCParams;
	GCParams.SourceObject = SourceCharacter;
	SourceASC->RemoveGC(SourceCharacter, SMTags::GameplayCue::Bass::Charge, GCParams);
}

void USMGA_Charge::OnChargeEndedSyncPoint()
{
	UAbilityTask_NetworkSyncPoint* SyncTask = UAbilityTask_NetworkSyncPoint::WaitNetSync(this, EAbilityTaskNetSyncType::BothWait);
	SyncTask->OnSync.AddDynamic(this, &ThisClass::K2_EndAbility);
	SyncTask->ReadyForActivation();
}
