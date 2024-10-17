// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_NoiseBreakIndicator.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "AbilitySystem/Task/SMAT_SkillIndicator.h"
#include "Actors/Character/Player/SMPlayerCharacterBase.h"
#include "Components/Character/SMHIC_Character.h"
#include "Data/DataAsset/Character/SMPlayerCharacterDataAsset.h"
#include "Data/DataTable/SMCharacterData.h"
#include "FunctionLibraries/SMDataTableFunctionLibrary.h"
#include "FunctionLibraries/SMTileFunctionLibrary.h"


USMGA_NoiseBreakIndicator::USMGA_NoiseBreakIndicator()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;

	AbilityTags.AddTag(SMTags::Ability::NoiseBreakIndicator);
}

void USMGA_NoiseBreakIndicator::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacterBase* OwnerCharacter = GetCharacter();
	USMHIC_Character* OwnerHIC = GetHIC();
	const USMPlayerCharacterDataAsset* OwnerDataAsset = GetDataAsset();
	const FSMCharacterNoiseBreakData* NoiseBreakData = USMDataTableFunctionLibrary::GetCharacterNoiseBreakData(OwnerCharacter->GetCharacterType());
	if (!OwnerCharacter || !OwnerHIC || !OwnerDataAsset || !NoiseBreakData)
	{
		EndAbilityByCancel();
		return;
	}

	NET_LOG(GetAvatarActor(), Log, TEXT("노이즈 브레이크 인디케이터 활성화"));

	const ESMTeam OwnerTeam = OwnerCharacter->GetTeam();
	UNiagaraSystem* NoiseBreakIndicator = OwnerDataAsset->NoiseBreakIndicatorFX.Contains(OwnerTeam) ? OwnerDataAsset->NoiseBreakIndicatorFX[OwnerTeam] : nullptr;
	const float NoiseBreakMaxDistance = NoiseBreakData->DistanceByTile * USMTileFunctionLibrary::DefaultTileSize; // 노이즈 브레이크 데이터를 통해 사거리를 가져옵니다.
	USMAT_SkillIndicator* SkillIndicatorTask = USMAT_SkillIndicator::SkillIndicator(this, NoiseBreakIndicator, USMAT_SkillIndicator::EIndicatorAlignment::Tile, NoiseBreakMaxDistance);
	SkillIndicatorTask->ReadyForActivation();

	// 무력화나 시간만료 등의 이유로 잡은 대상이 풀려나는 경우 즉시 취소합니다.
	OwnerHIC->OnHoldStateExit.AddUObject(this, &ThisClass::K2_EndAbility);
}

void USMGA_NoiseBreakIndicator::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	NET_LOG(GetAvatarActor(), Log, TEXT("노이즈 브레이크 인디케이터 종료"));

	if (USMHIC_Character* OwnerHIC = GetHIC())
	{
		OwnerHIC->OnHoldStateExit.RemoveAll(this);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
