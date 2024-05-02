// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayAbility_Immune.h"

#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "AbilitySystem/AttributeSets/SMCharacterAttributeSet.h"
#include "Characters/SMPlayerCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Utilities/SMLog.h"

USMGameplayAbility_Immune::USMGameplayAbility_Immune()
{
	AbilityTags = FGameplayTagContainer(SMTags::Ability::Immune);
}

void USMGameplayAbility_Immune::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensureAlways(SourceASC))
	{
		K2_CancelAbility();
		return;
	}

	UAbilityTask_WaitDelay* WaitdelayTask = UAbilityTask_WaitDelay::WaitDelay(this, ImmuneTime);
	if (ensureAlways(WaitdelayTask))
	{
		WaitdelayTask->OnFinish.AddDynamic(this, &ThisClass::OnFinishDelay);
		WaitdelayTask->ReadyForActivation();
	}

	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!ensureAlways(SourceCharacter))
	{
		K2_CancelAbility();
		return;
	}

	UCharacterMovementComponent* SourceMovement = SourceCharacter->GetCharacterMovement();
	if (!ensureAlways(SourceMovement))
	{
		K2_CancelAbility();
		return;
	}

	const USMCharacterAttributeSet* SourceAttributeSet = SourceASC->GetSet<USMCharacterAttributeSet>();
	if (!ensureAlways(SourceAttributeSet))
	{
		K2_CancelAbility();
		return;
	}

	const float MoveSpeedToAdd = (SourceAttributeSet->GetBaseMoveSpeed() * MoveSpeedMultiply) - SourceAttributeSet->GetBaseMoveSpeed();

	if (ActorInfo->IsLocallyControlled())
	{
		SourceMovement->MaxWalkSpeed += MoveSpeedToAdd;
	}

	if (ActorInfo->IsNetAuthority())
	{
		SourceCharacter->SetMaxWalkSpeed(SourceMovement->MaxWalkSpeed + MoveSpeedToAdd);
	}
}

void USMGameplayAbility_Immune::OnFinishDelay()
{
	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensureAlways(SourceASC))
	{
		K2_CancelAbility();
		return;
	}

	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!ensureAlways(SourceCharacter))
	{
		K2_CancelAbility();
		return;
	}

	UCharacterMovementComponent* SourceMovement = SourceCharacter->GetCharacterMovement();
	if (!ensureAlways(SourceMovement))
	{
		K2_CancelAbility();
		return;
	}

	const USMCharacterAttributeSet* SourceAttributeSet = SourceASC->GetSet<USMCharacterAttributeSet>();
	if (!ensureAlways(SourceAttributeSet))
	{
		K2_CancelAbility();
		return;
	}

	const float MoveSpeedToAdd = (SourceAttributeSet->GetBaseMoveSpeed() * MoveSpeedMultiply) - SourceAttributeSet->GetBaseMoveSpeed();

	if (CurrentActorInfo->IsLocallyControlled())
	{
		SourceMovement->MaxWalkSpeed -= MoveSpeedToAdd;
	}

	if (CurrentActorInfo->IsNetAuthority())
	{
		SourceCharacter->SetMaxWalkSpeed(SourceMovement->MaxWalkSpeed - MoveSpeedToAdd);
		SourceASC->RemoveTag(SMTags::Character::State::Immune);
	}

	K2_EndAbilityLocally();
}
