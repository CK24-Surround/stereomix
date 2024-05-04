// Copyright Surround, Inc. All Rights Reserved.


#include "SMGameplayAbility_TileTrigger.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Characters/SMPlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Tiles/SMTile.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"
#include "AbilitySystem/SMTags.h"

class UNiagaraSystem;

USMGameplayAbility_TileTrigger::USMGameplayAbility_TileTrigger()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = SMTags::Event::Tile::TileTrigger;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void USMGameplayAbility_TileTrigger::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMPlayerCharacter* SourceCharacter = GetSMPlayerCharacterFromActorInfo();
	if (!ensureAlways(SourceCharacter))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!ensureAlways(TriggerEventData))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	const FGameplayAbilityTargetData* TargetData = TriggerEventData->TargetData.Get(0);
	if (!ensureAlways(TargetData))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	const FHitResult* HitResult = TargetData->GetHitResult();
	if (!ensureAlways(HitResult))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ASMTile* Tile = Cast<ASMTile>(HitResult->GetActor());
	if (!ensureAlways(Tile))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	const UBoxComponent* TileBoxComponent = Tile->GetBoxComponent();
	if (!ensure(TileBoxComponent))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 트리거된 타일의 위치를 저장합니다.
	TriggerdTileLocation = Tile->GetTileLocation();

	// 팀을 저장합니다.
	SourceTeam = SourceCharacter->GetTeam();

	// 타일이 트리거 되야할 규모를 저장합니다.
	TileTriggerMagnitude = TriggerEventData->EventMagnitude;

	// 타일 가로 사이즈로 저장합니다.
	TileHorizonSize = TileBoxComponent->GetScaledBoxExtent().X * 2;

	NET_LOG(SourceCharacter, Log, TEXT("%s타일을 트리거 했습니다."), *Tile->GetName());
	TileTrigger();
}
