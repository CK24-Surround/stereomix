// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayAbility_Immune.h"

#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "GameplayEffectComponents/RemoveOtherGameplayEffectComponent.h"
#include "Utilities/SMLog.h"

USMGameplayAbility_Immune::USMGameplayAbility_Immune()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	AbilityTags = FGameplayTagContainer(SMTags::Ability::Immune);
	ActivationOwnedTags = FGameplayTagContainer(SMTags::Character::State::Immune);
}

void USMGameplayAbility_Immune::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilityTask_WaitDelay* WaitdelayTask = UAbilityTask_WaitDelay::WaitDelay(this, ImmuneTime);
	if (ensureAlways(WaitdelayTask))
	{
		WaitdelayTask->OnFinish.AddDynamic(this, &ThisClass::OnFinishDelay);
		WaitdelayTask->ReadyForActivation();
	}

	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensureAlways(SourceASC))
	{
		K2_CancelAbility();
		return;
	}

	UGameplayEffect* NewGE = MoveSpeedChangeGE->GetDefaultObject<UGameplayEffect>();
	if (!ensureAlways(NewGE))
	{
		return;
	}

	FGameplayEffectSpec* GESpec = new FGameplayEffectSpec(NewGE, MakeEffectContext(CurrentSpecHandle, CurrentActorInfo), 1.0f);
	GESpec->SetSetByCallerMagnitude(SMTags::Data::MoveSpeed, MoveSpeedMultiply);
	FGameplayEffectSpecHandle GESpecHandle(GESpec);
	(void)ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, GESpecHandle);
}

void USMGameplayAbility_Immune::OnFinishDelay()
{
	UGameplayEffect* NewGE = NewObject<UGameplayEffect>();
	if (!ensureAlways(NewGE))
	{
		K2_CancelAbility();
		return;
	}

	NewGE->DurationPolicy = EGameplayEffectDurationType::Instant;
	URemoveOtherGameplayEffectComponent& RemoveGEComponent = NewGE->FindOrAddComponent<URemoveOtherGameplayEffectComponent>();

	FGameplayEffectQuery GEQuery;
	GEQuery.EffectDefinition = MoveSpeedChangeGE;
	RemoveGEComponent.RemoveGameplayEffectQueries.AddUnique(GEQuery);

	FGameplayEffectSpec* GESpec = new FGameplayEffectSpec(NewGE, MakeEffectContext(CurrentSpecHandle, CurrentActorInfo), 1.0f);
	FGameplayEffectSpecHandle GESpecHandle(GESpec);
	(void)ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, GESpecHandle);

	K2_EndAbility();
}
