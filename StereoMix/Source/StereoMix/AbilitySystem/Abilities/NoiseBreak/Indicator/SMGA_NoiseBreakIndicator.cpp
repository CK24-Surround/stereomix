// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_NoiseBreakIndicator.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "AbilitySystem/Abilities/NoiseBreak/SMGA_NoiseBreak.h"
#include "AbilitySystem/Task/SMAT_SkillIndicator.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "FunctionLibraries/SMDataTableFunctionLibrary.h"
#include "GameInstance/SMGameInstance.h"
#include "HoldInteraction/SMHIC_Character.h"


USMGA_NoiseBreakIndicator::USMGA_NoiseBreakIndicator()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;

	AbilityTags.AddTag(SMTags::Ability::NoiseBreakIndicator);
}

void USMGA_NoiseBreakIndicator::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacterBase* SourceCharacter = GetCharacter();
	USMHIC_Character* SourceHIC = GetHIC<USMHIC_Character>();
	FSMCharacterNoiseBreakData* NoiseBreakData = USMDataTableFunctionLibrary::GetCharacterNoiseBreakData(SourceCharacter->GetCharacterType());
	if (!SourceCharacter || !SourceHIC || !NoiseBreakData)
	{
		EndAbilityByCancel();
		return;
	}

	UNiagaraComponent* NoiseBreakIndicator = SourceCharacter->GetNoiseBreakIndicator();
	const float NoiseBreakMaxDistance = NoiseBreakData->DistanceByTile * 150.0f; // 노이즈 브레이크 데이터를 통해 사거리를 가져옵니다.
	USMAT_SkillIndicator* SkillIndicatorTask = USMAT_SkillIndicator::SkillIndicator(this, NoiseBreakIndicator, NoiseBreakMaxDistance, true);
	SkillIndicatorTask->ReadyForActivation();

	SourceHIC->OnHoldStateExit.AddUObject(this, &ThisClass::K2_EndAbility);
}

void USMGA_NoiseBreakIndicator::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	USMHIC_Character* SourceHIC = GetHIC<USMHIC_Character>();
	if (SourceHIC)
	{
		SourceHIC->OnHoldStateExit.RemoveAll(this);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
