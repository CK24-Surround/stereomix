// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_Slash.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "AbilitySystem/Task/SMAT_ColliderOrientationForSlash.h"
#include "AbilitySystem/Task/SMAT_NextActionProccedCheck.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "Data/DataTable/SMCharacterData.h"

USMGA_Slash::USMGA_Slash()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

	ActivationOwnedTags.AddTag(SMTags::Character::State::SlashActivation);

	ActivationBlockedTags.AddTag(SMTags::Character::State::Charge);

	if (FSMCharacterAttackData* AttackData = GetAttackData(ESMCharacterType::Bass))
	{
		Damage = AttackData->Damage;
		MaxDistanceByTile = AttackData->DistanceByTile;
		Angle = AttackData->SpreadAngle;
	}
}

void USMGA_Slash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	const USMPlayerCharacterDataAsset* SourceDataAsset = GetDataAsset();
	if (!SourceDataAsset)
	{
		EndAbilityByCancel();
		return;
	}

	const ESMTeam SourceTeam = SourceCharacter->GetTeam();

	if (IsLocallyControlled())
	{
		SourceCharacter->FocusToCursor();
	}

	const FName TaskName = TEXT("MontageTask");
	UAnimMontage* Montage = SourceDataAsset->AttackMontage[SourceTeam];
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TaskName, Montage, 1.0f, NAME_None, false);
	MontageTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
	MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
	MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
	MontageTask->ReadyForActivation();

	if (IsLocallyControlled())
	{
		UAbilityTask_WaitGameplayEvent* SlashJudgeStartEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::AnimNotify::Attack::AttackStart);
		SlashJudgeStartEventTask->EventReceived.AddDynamic(this, &ThisClass::OnSlashJudgeStartCallback);
		SlashJudgeStartEventTask->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* CanInputEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::AnimNotify::Attack::CanInput);
		CanInputEventTask->EventReceived.AddDynamic(this, &ThisClass::CanInputCallback);
		CanInputEventTask->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* CanProceedNextActionEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::AnimNotify::Attack::CanProceedNextAction);
		CanProceedNextActionEventTask->EventReceived.AddDynamic(this, &ThisClass::CanProceedNextActionCallback);
		CanProceedNextActionEventTask->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* LeftSlashEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::AnimNotify::Attack::LeftSlashNext);
		LeftSlashEventTask->EventReceived.AddDynamic(this, &ThisClass::LeftSlashNextCallback);
		LeftSlashEventTask->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* RightSlashEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::AnimNotify::Attack::RightSlashNext);
		RightSlashEventTask->EventReceived.AddDynamic(this, &ThisClass::RightSlashNextCallback);
		RightSlashEventTask->ReadyForActivation();

		USMAT_NextActionProccedCheck* NextActionProccedTask = USMAT_NextActionProccedCheck::NextActionProccedCheck(this);
		NextActionProccedTask->OnNextActionProcced.BindUObject(this, &ThisClass::OnNextActionProcced);
		NextActionProccedTask->ReadyForActivation();
	}

	K2_CommitAbilityCost();
}

void USMGA_Slash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	bCanInput = false;
	bIsInput = false;
	bCanProceedNextAction = false;
	bIsLeftSlashNext = true;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USMGA_Slash::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (!K2_CommitAbility())
	{
		return;
	}

	if (bCanInput)
	{
		bCanInput = false;
		bIsInput = true;
	}
}

void USMGA_Slash::OnSlashJudgeStartCallback(FGameplayEventData Payload)
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	USMAbilitySystemComponent* SourceASC = GetASC<USMAbilitySystemComponent>();
	if (!SourceCharacter || !SourceASC)
	{
		return;
	}

	const float MaxDistance = MaxDistanceByTile * 150.0f;
	USMAT_ColliderOrientationForSlash* ColliderOrientationForSlashTask = USMAT_ColliderOrientationForSlash::ColliderOrientationForSlash(this, MaxDistance, Angle, TotalSlashTime, bShowDebug);
	ColliderOrientationForSlashTask->OnSlashHit.BindUObject(this, &ThisClass::OnSlashHit);
	ColliderOrientationForSlashTask->ReadyForActivation();

	FGameplayCueParameters CueParams;
	CueParams.Normal = SourceCharacter->GetActorRotation().Vector();
	CueParams.RawMagnitude = bIsLeftSlashNext ? 0.0f : 1.0f;
	SourceASC->ExecuteGC(SourceCharacter, SMTags::GameplayCue::Bass::Slash, CueParams);
}

void USMGA_Slash::ServerSendSlashDirection_Implementation(bool bNewIsLeftSlashNext)
{
	bIsLeftSlashNext = bNewIsLeftSlashNext;
}

void USMGA_Slash::OnNextActionProcced()
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	const USMPlayerCharacterDataAsset* SourceDataAsset = GetDataAsset();
	if (!SourceDataAsset)
	{
		EndAbilityByCancel();
		return;
	}

	if (IsLocallyControlled())
	{
		SourceCharacter->FocusToCursor();
	}

	const FName SectionName = bIsLeftSlashNext ? TEXT("Left") : TEXT("Right");
	MontageJumpToSection(SectionName);
	ServerRPCApplyCost();
}

void USMGA_Slash::ServerRPCApplyCost_Implementation()
{
	K2_CommitAbilityCost();
}

void USMGA_Slash::OnSlashHit(AActor* TargetActor)
{
	NET_LOG(GetAvatarActor(), Log, TEXT("%s 적중"), *GetNameSafe(TargetActor));

	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	USMAbilitySystemComponent* SourceASC = GetASC<USMAbilitySystemComponent>();
	ASMPlayerCharacterBase* TargetCharacter = Cast<ASMPlayerCharacterBase>(TargetActor);
	if (!SourceCharacter || !SourceASC || !TargetCharacter)
	{
		return;
	}

	ServerRPCSlashHit(TargetCharacter);

	FGameplayCueParameters CueParams;
	CueParams.Instigator = SourceCharacter;
	SourceASC->ExecuteGC(TargetCharacter, SMTags::GameplayCue::Bass::SlashHit, CueParams);

	if (!K2_HasAuthority())
	{
		// TargetCharacter->PredictHPChange(-Damage);
	}
}

void USMGA_Slash::ServerRPCSlashHit_Implementation(AActor* TargetActor)
{
	UAbilitySystemComponent* SourceASC = GetASC();
	const USMPlayerCharacterDataAsset* SourceDataAsset = GetDataAsset();
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!SourceASC || !SourceDataAsset || !TargetASC)
	{
		return;
	}

	FGameplayEffectSpecHandle GESpecHandle = MakeOutgoingGameplayEffectSpec(SourceDataAsset->DamageGE);
	if (GESpecHandle.IsValid())
	{
		GESpecHandle.Data->SetSetByCallerMagnitude(SMTags::Data::Damage, Damage);
	}

	SourceASC->BP_ApplyGameplayEffectSpecToTarget(GESpecHandle, TargetASC);

	ISMDamageInterface* TargetDamageInterface = Cast<ISMDamageInterface>(TargetActor);
	if (TargetDamageInterface)
	{
		TargetDamageInterface->SetLastAttackInstigator(GetAvatarActor());
	}
}
