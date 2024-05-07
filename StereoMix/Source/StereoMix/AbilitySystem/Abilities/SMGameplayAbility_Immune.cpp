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
		EndAbilityByCancel();
		return;
	}

	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!ensureAlways(SourceCharacter))
	{
		EndAbilityByCancel();
		return;
	}

	UCharacterMovementComponent* SourceMovement = SourceCharacter->GetCharacterMovement();
	if (!ensureAlways(SourceMovement))
	{
		EndAbilityByCancel();
		return;
	}

	const USMCharacterAttributeSet* SourceAttributeSet = SourceASC->GetSet<USMCharacterAttributeSet>();
	if (!ensureAlways(SourceAttributeSet))
	{
		EndAbilityByCancel();
		return;
	}

	// 더해줘야할 이동속도를 구합니다.
	const float MoveSpeedToAdd = (SourceAttributeSet->GetBaseMoveSpeed() * MoveSpeedMultiply) - SourceAttributeSet->GetBaseMoveSpeed();

	// 클라이언트의 경우 예측 실행합니다.
	if (ActorInfo->IsLocallyControlled())
	{
		SourceMovement->MaxWalkSpeed += MoveSpeedToAdd;
	}

	// 서버의 경우 실제로 반영합니다.
	if (ActorInfo->IsNetAuthority())
	{
		SourceCharacter->SetMaxWalkSpeed(SourceMovement->MaxWalkSpeed + MoveSpeedToAdd);
	}

	// 면역 시간만큼 기다립니다.
	UAbilityTask_WaitDelay* WaitdelayTask = UAbilityTask_WaitDelay::WaitDelay(this, ImmuneTime);
	if (ensureAlways(WaitdelayTask))
	{
		WaitdelayTask->OnFinish.AddDynamic(this, &ThisClass::OnFinishDelay);
		WaitdelayTask->ReadyForActivation();
	}
}

void USMGameplayAbility_Immune::OnFinishDelay()
{
	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensureAlways(SourceASC))
	{
		EndAbilityByCancel();
		return;
	}

	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!ensureAlways(SourceCharacter))
	{
		EndAbilityByCancel();
		return;
	}

	UCharacterMovementComponent* SourceMovement = SourceCharacter->GetCharacterMovement();
	if (!ensureAlways(SourceMovement))
	{
		EndAbilityByCancel();
		return;
	}

	const USMCharacterAttributeSet* SourceAttributeSet = SourceASC->GetSet<USMCharacterAttributeSet>();
	if (!ensureAlways(SourceAttributeSet))
	{
		EndAbilityByCancel();
		return;
	}

	// 감소시켜줘야할 이동속도를 구합니다.
	const float MoveSpeedToAdd = (SourceAttributeSet->GetBaseMoveSpeed() * MoveSpeedMultiply) - SourceAttributeSet->GetBaseMoveSpeed();

	// 클라이언트의 경우 예측 실행합니다.
	if (CurrentActorInfo->IsLocallyControlled())
	{
		SourceMovement->MaxWalkSpeed -= MoveSpeedToAdd;
	}

	// 서버의 경우 실제로 반영합니다. 그리고 면역 태그도 제거해줍니다.
	if (CurrentActorInfo->IsNetAuthority())
	{
		SourceCharacter->SetMaxWalkSpeed(SourceMovement->MaxWalkSpeed - MoveSpeedToAdd);
		SourceASC->RemoveTag(SMTags::Character::State::Immune);
	}

	if (CurrentActorInfo->IsNetAuthority())
	{
		K2_EndAbility();
	}
	else
	{
		K2_EndAbilityLocally();
	}
}
