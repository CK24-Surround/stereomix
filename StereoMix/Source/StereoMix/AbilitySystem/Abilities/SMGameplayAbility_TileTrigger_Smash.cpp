// Fill out your copyright notice in the Description page of Project Settings.


#include "SMGameplayAbility_TileTrigger_Smash.h"

#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "Tiles/SMTile.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"
#include "AbilitySystem/SMTags.h"
#include "Engine/OverlapResult.h"

class ASMTile;

void USMGameplayAbility_TileTrigger_Smash::TileTrigger()
{
	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensureAlways(SourceASC))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	// 타일 트리거 규모를 트리거 횟수로 치환해줍니다.
	MaxTriggerCount = static_cast<int32>(TileTriggerMagnitude);

	// 타일 트리거마다 딜레이되야할 시간을 구합니다.
	DelayTime = TotalTriggerTime / (MaxTriggerCount - 1);

	ProcessContinuousTileTrigger();
}

void USMGameplayAbility_TileTrigger_Smash::ProcessContinuousTileTrigger()
{
	if (CurrentTriggerCount < MaxTriggerCount)
	{
		TArray<FOverlapResult> OverlapResults;
		FVector Start = TriggerdTileLocation;
		FVector HalfExtend(Range, Range, 100.0);
		FCollisionShape CollisionShape = FCollisionShape::MakeBox(HalfExtend);
		const bool bSuccess = GetWorld()->OverlapMultiByChannel(OverlapResults, Start, FQuat::Identity, SMCollisionTraceChannel::TileAction, CollisionShape);
		if (bSuccess)
		{
			for (const auto& OverlapResult : OverlapResults)
			{
				ASMTile* Tile = Cast<ASMTile>(OverlapResult.GetActor());
				if (Tile)
				{
					Tile->TileTrigger(SourceTeam);
				}
			}
		}

		DrawDebugBox(GetWorld(), Start, HalfExtend, FColor::Turquoise, false, 2.0f);

		// 다음 트리거를 위해 값을 더해줍니다. 더한 값이 이미 Max라면 더 이상 타이머를 실행시키지 않습니다.
		Range += TileHorizonSize;
		++CurrentTriggerCount;
		if (CurrentTriggerCount < MaxTriggerCount)
		{
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::ProcessContinuousTileTrigger, DelayTime, false);
		}
		else
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		}
	}
}
