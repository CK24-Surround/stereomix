// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_NoiseBreakIndicator.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "AbilitySystem/Task/SMAT_SkillIndicator.h"
#include "Characters/Component/SMHIC_Character.h"
#include "Characters/Player/SMPlayerCharacterBase.h"
#include "FunctionLibraries/SMDataTableFunctionLibrary.h"
#include "GameInstance/SMGameInstance.h"


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

	NET_LOG(GetAvatarActor(), Log, TEXT("노이즈 브레이크 인디케이터 활성화"));
	UNiagaraComponent* NoiseBreakIndicator = SourceCharacter->GetNoiseBreakIndicator();
	const float NoiseBreakMaxDistance = NoiseBreakData->DistanceByTile * 150.0f; // 노이즈 브레이크 데이터를 통해 사거리를 가져옵니다.
	USMAT_SkillIndicator* SkillIndicatorTask = USMAT_SkillIndicator::SkillIndicator(this, NoiseBreakIndicator, NoiseBreakMaxDistance, true);
	SkillIndicatorTask->ReadyForActivation();

	// 무력화나 시간만료 등의 이유로 잡은 대상이 풀려나는 경우 즉시 취소합니다.
	SourceHIC->OnHoldStateExit.AddUObject(this, &ThisClass::K2_EndAbility);
}

void USMGA_NoiseBreakIndicator::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	NET_LOG(GetAvatarActor(), Log, TEXT("노이즈 브레이크 인디케이터 종료"));
	USMHIC_Character* SourceHIC = GetHIC<USMHIC_Character>();
	if (SourceHIC)
	{
		SourceHIC->OnHoldStateExit.RemoveAll(this);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
