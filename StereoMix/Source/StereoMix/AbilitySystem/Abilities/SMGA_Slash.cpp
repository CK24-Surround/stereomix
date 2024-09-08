// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_Slash.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "Task/SMAT_NextActionProccedCheck.h"

USMGA_Slash::USMGA_Slash()
{
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
	MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::SlashEnded);
	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::SlashEnded);
	MontageTask->ReadyForActivation();

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

void USMGA_Slash::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (bCanInput)
	{
		NET_LOG(GetAvatarActor(), Warning, TEXT("입력!"));
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

void USMGA_Slash::SlashEnded()
{
	K2_EndAbility();
}

void USMGA_Slash::OnSlashJudgeStartCallback(FGameplayEventData Payload)
{
	
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
	NET_LOG(GetAvatarActor(), Warning, TEXT("%s"), *SectionName.ToString());
	MontageJumpToSection(SectionName);
}
