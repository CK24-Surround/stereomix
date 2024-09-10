// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_ImpactArrow.h"

#include "EnhancedInputComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Controllers/SMGamePlayerController.h"
#include "Data/SMControlData.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"

class ASMPlayerController;

USMGA_ImpactArrow::USMGA_ImpactArrow()
{
	ActivationOwnedTags.AddTag(SMTags::Character::State::ImpactArrow);
}

void USMGA_ImpactArrow::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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

	if (IsLocallyControlled())
	{
		ASMGamePlayerController* SourcePC = SourceCharacter->GetController<ASMGamePlayerController>();
		const USMControlData* ControlData = SourcePC->GetControlData();
		if (SourcePC && ControlData)
		{
			InputComponent = NewObject<UEnhancedInputComponent>(SourcePC);
			InputComponent->RegisterComponent();
			InputComponent->BindAction(ControlData->AttackAction, ETriggerEvent::Started, this, &ThisClass::Shot);

			SourcePC->PushInputComponent(InputComponent);
		}
	}

	const FName TaskName = TEXT("MontageTask");
	ESMTeam SourceTeam = SourceCharacter->GetTeam();
	UAnimMontage* Montage = SourceDataAsset->SkillMontage[SourceTeam];
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TaskName, Montage);
	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
	MontageTask->ReadyForActivation();

	K2_CommitAbility();
}

void USMGA_ImpactArrow::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (SourceCharacter)
	{
		ASMGamePlayerController* SourcePC = SourceCharacter->GetController<ASMGamePlayerController>();
		if (SourcePC)
		{
			if (InputComponent)
			{
				SourcePC->PopInputComponent(InputComponent);
				InputComponent = nullptr;
			}
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USMGA_ImpactArrow::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	K2_CancelAbility();
}

void USMGA_ImpactArrow::Shot()
{
	NET_LOG(GetAvatarActor(), Warning, TEXT("발사"));
	const FName SectionName = TEXT("End");
	MontageJumpToSection(SectionName);
}
