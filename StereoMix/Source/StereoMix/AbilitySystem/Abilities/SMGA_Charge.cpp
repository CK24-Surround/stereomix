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
}

void USMGA_Charge::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!SourceCharacter)
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

	const USMBassCharacterDataAsset* SourceDataAsset = SourceCharacter->GetDataAsset<USMBassCharacterDataAsset>();
	if (!ensureAlways(SourceDataAsset))
	{
		EndAbilityByCancel();
		return;
	}

	K2_CommitAbility();

	const FName TaskName = TEXT("ChargeMontage");
	UAnimMontage* ChargeMontage = SourceDataAsset->SkillMontage[SourceCharacter->GetTeam()];
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

	OriginalCollisionProfileName = SourceCapsule->GetCollisionProfileName();
	SourceCapsule->SetCollisionProfileName(SMCollisionProfileName::Ghost);
}

void USMGA_Charge::OnChargeBlocked()
{
	const FName SectionName = TEXT("End");
	MontageJumpToSection(SectionName);
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
