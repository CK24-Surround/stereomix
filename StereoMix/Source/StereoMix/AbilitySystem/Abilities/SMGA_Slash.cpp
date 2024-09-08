// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_Slash.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "Task/SMAT_ColliderOrientationForSlash.h"
#include "Task/SMAT_NextActionProccedCheck.h"

USMGA_Slash::USMGA_Slash()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	bReplicateInputDirectly = true;

	ActivationOwnedTags.AddTag(SMTags::Character::State::SlashActivation);

	ActivationBlockedTags.AddTag(SMTags::Ability::Activation::Hold);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Hold);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Holded);
	ActivationBlockedTags.AddTag(SMTags::Character::State::NoiseBreaking);
	ActivationBlockedTags.AddTag(SMTags::Character::State::NoiseBreaked);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Neutralize);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Immune);
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

	const USMPlayerCharacterDataAsset* SourceDataAsset = SourceCharacter->GetDataAsset();
	if (!SourceDataAsset)
	{
		EndAbilityByCancel();
		return;
	}

	const ESMTeam SourceTeam = SourceCharacter->GetTeam();

	if (IsLocallyControlled())
	{
		FocusToCursor();
	}

	const FName TaskName = TEXT("MontageTask");
	UAnimMontage* Montage = SourceDataAsset->AttackMontage[SourceTeam];
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TaskName, Montage, 1.0f, NAME_None, false);
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

void USMGA_Slash::FocusToCursor()
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!SourceCharacter)
	{
		return;
	}

	APlayerController* SourceController = SourceCharacter->GetController<APlayerController>();
	if (!SourceController)
	{
		return;
	}

	const FVector CursorTargetingPoint = SourceCharacter->GetCursorTargetingPoint();
	const FVector SourceLocation = SourceCharacter->GetActorLocation();
	const FVector Direction = (CursorTargetingPoint - SourceLocation).GetSafeNormal();
	const FRotator NewRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
	SourceController->SetControlRotation(NewRotation);
}

void USMGA_Slash::OnSlashJudgeStartCallback(FGameplayEventData Payload)
{
	USMAT_ColliderOrientationForSlash* ColliderOrientationForSlashTask = USMAT_ColliderOrientationForSlash::ColliderOrientationForSlash(this, Range, Angle, TotalSlashTime, bShowDebug);
	ColliderOrientationForSlashTask->OnSlashHit.BindUObject(this, &ThisClass::OnSlashHit);
	ColliderOrientationForSlashTask->ReadyForActivation();
}

void USMGA_Slash::OnNextActionProcced()
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	const USMPlayerCharacterDataAsset* SourceDataAsset = SourceCharacter->GetDataAsset();
	if (!SourceDataAsset)
	{
		EndAbilityByCancel();
		return;
	}

	if (IsLocallyControlled())
	{
		FocusToCursor();
	}

	const FName SectionName = bIsLeftSlashNext ? TEXT("Left") : TEXT("Right");
	MontageJumpToSection(SectionName);
	ServerRPCApplyCost();
}

void USMGA_Slash::OnSlashHit(AActor* TargetActor)
{
	NET_LOG(GetAvatarActor(), Log, TEXT("%s 적중"), *GetNameSafe(TargetActor));

	ASMPlayerCharacterBase* TargetCharacter = Cast<ASMPlayerCharacterBase>(TargetActor);
	if (!TargetCharacter)
	{
		return;
	}

	TargetCharacter->PredictHPChange(-Damage);
	ServerRPCSlashHit(TargetActor);
}

void USMGA_Slash::ServerRPCApplyCost_Implementation()
{
	K2_CommitAbilityCost();
}

void USMGA_Slash::ServerRPCSlashHit_Implementation(AActor* TargetActor)
{
	UAbilitySystemComponent* SourceASC = GetASC();
	if (!SourceASC)
	{
		return;
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!TargetASC)
	{
		return;
	}

	const USMPlayerCharacterDataAsset* SourceDataAsset = GetDataAsset();
	if (!SourceDataAsset)
	{
		return;
	}

	FGameplayEffectSpecHandle GESpecHandle = MakeOutgoingGameplayEffectSpec(SourceDataAsset->DamageGE);
	if (GESpecHandle.IsValid())
	{
		GESpecHandle.Data->SetSetByCallerMagnitude(SMTags::Data::Damage, Damage);
	}

	SourceASC->BP_ApplyGameplayEffectSpecToTarget(GESpecHandle, TargetASC);
}
