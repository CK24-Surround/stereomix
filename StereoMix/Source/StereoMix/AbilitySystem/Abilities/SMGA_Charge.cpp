// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_Charge.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "Data/Character/SMBassCharacterDataAsset.h"
#include "Task/SMAT_WaitChargeBlocked.h"
#include "Utilities/SMCollision.h"

USMGA_Charge::USMGA_Charge()
{
	ActivationOwnedTags.AddTag(SMTags::Character::State::Charge);

	ActivationBlockedTags.AddTag(SMTags::Character::State::Hold);
	ActivationBlockedTags.AddTag(SMTags::Character::State::Holded);
	ActivationBlockedTags.AddTag(SMTags::Character::State::NoiseBreaking);
	ActivationBlockedTags.AddTag(SMTags::Character::State::NoiseBreaked);
}

void USMGA_Charge::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!ensureAlways(SourceCharacter))
	{
		EndAbilityByCancel();
		return;
	}

	const USMBassCharacterDataAsset* SourceDataAsset = SourceCharacter->GetDataAsset<USMBassCharacterDataAsset>();
	if (!ensureAlways(SourceDataAsset))
	{
		EndAbilityByCancel();
		return;
	}

	K2_CommitAbility();

	// TODO: 테스트코드
	{
		UAbilitySystemComponent* SourceASC = GetASC();
		if (!SourceASC)
		{
			EndAbilityByCancel();
			return;
		}

		if (IsLocallyControlled())
		{
			const FVector SourceLocation = SourceCharacter->GetActorLocation();
			const FVector TargetLocation = SourceCharacter->GetCursorTargetingPoint();
			FGameplayAbilityTargetData_LocationInfo* NewData = new FGameplayAbilityTargetData_LocationInfo();
			NewData->SourceLocation.LiteralTransform.SetLocation(SourceLocation);
			NewData->TargetLocation.LiteralTransform.SetLocation(TargetLocation);
			FGameplayAbilityTargetDataHandle TargetDataHandle(NewData);

			FScopedPredictionWindow ScopedPrediction(SourceASC);
			FPredictionKey PredictionKey = CurrentActivationInfo.GetActivationPredictionKey();
			SourceASC->CallServerSetReplicatedTargetData(CurrentSpecHandle, PredictionKey, TargetDataHandle, FGameplayTag(), PredictionKey);

			ChargeStart(SourceLocation, TargetLocation);
		}

		if (K2_HasAuthority())
		{
			FAbilityTargetDataSetDelegate& Delegate = SourceASC->AbilityTargetDataSetDelegate(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());
			Delegate.AddUObject(this, &ThisClass::OnReceiveTargetData);
		}
	}

	const FName TaskName = TEXT("ChargeMontage");
	UAnimMontage* ChargeMontage = SourceDataAsset->ChargeMontage[SourceCharacter->GetTeam()];
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TaskName, ChargeMontage);
	if (!ensureAlways(MontageTask))
	{
		EndAbilityByCancel();
		return;
	}
	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnChargeEnded);
	MontageTask->ReadyForActivation();

	USMAT_WaitChargeBlocked* ChargeBlockedTask = USMAT_WaitChargeBlocked::WaitChargeBlocked(this, SourceCharacter);
	if (!ensureAlways(ChargeBlockedTask))
	{
		EndAbilityByCancel();
		return;
	}
	ChargeBlockedTask->OnChargeBlocked.BindUObject(this, &ThisClass::OnChargeBlocked);
	ChargeBlockedTask->ReadyForActivation();
}

void USMGA_Charge::OnReceiveTargetData(const FGameplayAbilityTargetDataHandle& GameplayAbilityTargetDataHandle, FGameplayTag GameplayTag)
{
	if (GameplayAbilityTargetDataHandle.IsValid(0))
	{
		const FGameplayAbilityTargetData* TargetData = GameplayAbilityTargetDataHandle.Get(0);
		if (TargetData)
		{
			const FVector SourceLocation = TargetData->GetOrigin().GetLocation();
			const FVector TargetLocation = TargetData->GetEndPoint();
			ChargeStart(SourceLocation, TargetLocation);
		}
	}
}

void USMGA_Charge::ChargeStart(const FVector& SourceLocation, const FVector& TargetLocation)
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!ensureAlways(SourceCharacter))
	{
		EndAbilityByCancel();
		return;
	}

	UCapsuleComponent* SourceCapsule = SourceCharacter->GetCapsuleComponent();
	if (!SourceCapsule)
	{
		EndAbilityByCancel();
		return;
	}

	OriginalCollisionProfileName = SourceCapsule->GetCollisionProfileName();
	SourceCapsule->SetCollisionProfileName(SMCollisionProfileName::Ghost);

	const FVector Direction = (TargetLocation - SourceLocation).GetSafeNormal();
	const FName TaskName = TEXT("RootMotionForce");
	RootMotionForceTask = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(this, TaskName, Direction, 750.0f, 10.0f, false, nullptr, ERootMotionFinishVelocityMode::SetVelocity, FVector::ZeroVector, 0.0f, true);
	if (!ensureAlways(RootMotionForceTask))
	{
		EndAbilityByCancel();
		return;
	}
	RootMotionForceTask->ReadyForActivation();
}

void USMGA_Charge::OnChargeBlocked()
{
	const FName SectionName = TEXT("End");
	MontageJumpToSection(SectionName);
	RootMotionForceTask->EndTask();
}

void USMGA_Charge::OnChargeEnded()
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!ensureAlways(SourceCharacter))
	{
		EndAbilityByCancel();
		return;
	}

	UCapsuleComponent* SourceCapsule = SourceCharacter->GetCapsuleComponent();
	if (!SourceCapsule)
	{
		EndAbilityByCancel();
		return;
	}

	SourceCapsule->SetCollisionProfileName(OriginalCollisionProfileName);

	K2_EndAbilityLocally();
}
