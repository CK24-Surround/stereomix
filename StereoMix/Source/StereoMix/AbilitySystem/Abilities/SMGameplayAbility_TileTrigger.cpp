// Fill out your copyright notice in the Description page of Project Settings.


#include "SMGameplayAbility_TileTrigger.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Characters/SMPlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SMTeamComponent.h"
#include "Tiles/SMTile.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"
#include "Utilities/SMTags.h"

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

	FHitResult HitResult;
	const FVector Start = TargetData->GetEndPoint();
	const FVector End = Start + (-FVector::UpVector * 1000.0f);
	const FCollisionQueryParams Param(SCENE_QUERY_STAT(TileTrace), false);
	const FCollisionShape CollisionShape = FCollisionShape::MakeSphere(25.0f);
	const bool bSuccess = GetWorld()->SweepSingleByChannel(HitResult, Start, End, FQuat::Identity, SMCollisionTraceChannel::TileAction, CollisionShape, Param);
	DrawDebugLine(GetWorld(), Start, End, FColor::Silver, false, 3.0f);

	if (bSuccess)
	{
		ASMTile* Tile = Cast<ASMTile>(HitResult.GetActor());
		if (Tile)
		{
			TriggerdTileLocation = Tile->GetTileLocation();

			const UBoxComponent* TileBoxComponent = Tile->GetBoxComponent();
			if (!ensure(TileBoxComponent))
			{
				EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			}

			TileHorizonSize = TileBoxComponent->GetScaledBoxExtent().X * 2;

			USMTeamComponent* SourceTeamComponent = SourceCharacter->GetTeamComponent();
			if (!ensure(SourceTeamComponent))
			{
				EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			}

			SourceTeam = SourceTeamComponent->GetTeam();

			NET_LOG(SourceCharacter, Log, TEXT("%s타일을 트리거 했습니다."), *Tile->GetName());
			TileTrigger(TriggerEventData->OptionalObject);
			return;
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}
