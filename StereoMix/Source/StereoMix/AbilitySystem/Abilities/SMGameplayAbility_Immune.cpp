// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayAbility_Immune.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilitySystem/AttributeSets/SMCharacterAttributeSet.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/SMPlayerCharacter.h"
#include "FMODBlueprintStatics.h"
#include "Utilities/SMLog.h"

USMGameplayAbility_Immune::USMGameplayAbility_Immune()
{
	AbilityTags = FGameplayTagContainer(SMTags::Ability::Immune);
}

void USMGameplayAbility_Immune::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!SourceCharacter)
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

	// 면역 이펙트, 머티리얼을 적용합니다.
	if (ActorInfo->IsNetAuthority())
	{
		SourceCharacter->SetCharacterMoveTrailState(EMoveTrailState::Immune);
	}

	SourceASC->ExecuteGameplayCue(SMTags::GameplayCue::ImmuneMaterialApply_ElectricGuitar);

	if (ActorInfo->IsLocallyControlled())
	{
		UFMODBlueprintStatics::PlayEvent2D(SourceCharacter, ImmuneSound, true);
	}
}

void USMGameplayAbility_Immune::OnFinishDelay()
{
	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!SourceCharacter)
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

	// 면역 종료 이펙트, 머티리얼을 적용합니다.
	if (CurrentActorInfo->IsNetAuthority())
	{
		SourceCharacter->SetCharacterMoveTrailState(EMoveTrailState::Default);
	}

	FGameplayCueParameters GCParams;
	GCParams.TargetAttachComponent = SourceCharacter->GetMesh();
	SourceASC->ExecuteGameplayCue(SMTags::GameplayCue::ImmuneEnd, GCParams);
	SourceASC->ExecuteGameplayCue(SMTags::GameplayCue::ImmuneMaterialReset);

	if (CurrentActorInfo->IsNetAuthority())
	{
		K2_EndAbility();
	}
	else
	{
		K2_EndAbilityLocally();
	}
}
