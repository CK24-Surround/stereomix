// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_Slash.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"

USMGA_Slash::USMGA_Slash()
{
	ActivationOwnedTags.AddTag(SMTags::Character::State::SlashActivation);
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
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TaskName, Montage);
	MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::SlashEnded);
	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::SlashEnded);
	MontageTask->ReadyForActivation();
}

void USMGA_Slash::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (true)
	{
		
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
