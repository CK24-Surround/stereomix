// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayAbility_CatchBuff.h"

#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSets/SMCharacterAttributeSet.h"
#include "Characters/SMPlayerCharacter.h"
#include "Components/SMCatchInteractionComponent_Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void USMGameplayAbility_CatchBuff::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!ensureAlways(SourceCharacter))
	{
		EndAbilityByCancel();
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensureAlways(SourceASC))
	{
		EndAbilityByCancel();
		return;
	}

	USMCatchInteractionComponent_Character* SourceCIC = Cast<USMCatchInteractionComponent_Character>(SourceCharacter->GetCatchInteractionComponent());
	if (!ensureAlways(SourceCIC))
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

	// 잡기가 풀릴때까지 버프가 지속됩니다.
	SourceCIC->OnCatchRelease.AddUObject(this, &ThisClass::OnCatchRelease);
}

void USMGameplayAbility_CatchBuff::OnCatchRelease()
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
	}

	K2_EndAbilityLocally();
}
