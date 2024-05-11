// Copyright Surround, Inc. All Rights Reserved.


#include "SMCatchableItem_AttributeChanger.h"

#include "Components/BoxComponent.h"
#include "Components/SMCatchInteractionComponent_CatchableItem_AttributeChanger.h"
#include "Components/SMTeamComponent.h"
#include "Engine/OverlapResult.h"
#include "Tiles/SMTile.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"


ASMCatchableItem_AttributeChanger::ASMCatchableItem_AttributeChanger()
{
	CIC = CreateDefaultSubobject<USMCatchInteractionComponent_CatchableItem_AttributeChanger>(TEXT("CIC"));

	TeamComponent = CreateDefaultSubobject<USMTeamComponent>(TEXT("TeamComponent"));
}

USMCatchInteractionComponent* ASMCatchableItem_AttributeChanger::GetCatchInteractionComponent()
{
	return CIC;
}

void ASMCatchableItem_AttributeChanger::ActivateItem(ESMTeam SourceTeam)
{
	TeamComponent->SetTeam(SourceTeam);

	TriggerCount = Duration / Interval;
	Amount = TotalAmount / TriggerCount;

	TriggerCountTimerStart();
}

void ASMCatchableItem_AttributeChanger::TriggerCountTimerStart()
{
	TriggerData.CurrentTriggerCount = 0;

	TriggerCountTimerCallback();
}

void ASMCatchableItem_AttributeChanger::TriggerCountTimerCallback()
{
	if (TriggerData.CurrentTriggerCount++ < TriggerCount)
	{
		HandleApplyItem();

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::TriggerCountTimerCallback, Interval, false);
	}
}

void ASMCatchableItem_AttributeChanger::HandleApplyItem()
{
	TArray<TWeakObjectPtr<AActor>> ActorsToApplyItem = ScanActorsToApplyItem();

	TArray<TWeakObjectPtr<AActor>> ConfirmedActorsToApplyItem;
	for (const auto& ActorToApplyItem : ActorsToApplyItem)
	{
		if (!ActorToApplyItem.Get())
		{
			continue;
		}

		if (IsValidActorToApplyItem(ActorToApplyItem.Get()))
		{
			ConfirmedActorsToApplyItem.Add(ActorToApplyItem);
		}
	}

	NET_LOG(this, Warning, TEXT("아이템이 적용될 캐릭터 리스트"));
	for (const auto& ConfirmedActorToApplyItem : ConfirmedActorsToApplyItem)
	{
		NET_LOG(this, Warning, TEXT("%s"), *ConfirmedActorToApplyItem->GetName());
	}
}

TArray<TWeakObjectPtr<AActor>> ASMCatchableItem_AttributeChanger::ScanActorsToApplyItem()
{
	TArray<TWeakObjectPtr<AActor>> Result;
	for (const auto& TriggeredTile : TriggeredTiles)
	{
		if (!ensureAlways(TriggeredTile.Get()))
		{
			continue;
		}

		TArray<FOverlapResult> OverlapResults;
		FVector Start = TriggeredTile->GetTileLocation();
		UBoxComponent* TileBoxComponent = TriggeredTile->GetBoxComponent();
		float halfTileHorizonSize = TileBoxComponent->GetScaledBoxExtent().X;

		FVector CollisionHalfExtend;
		if (halfTileHorizonSize > 0.0f)
		{
			const float Height = 50.0f;
			CollisionHalfExtend = FVector(halfTileHorizonSize, halfTileHorizonSize, Height);
			Start.Z += Height;
		}

		const FCollisionShape CollisionShape = FCollisionShape::MakeBox(CollisionHalfExtend);
		const FCollisionQueryParams Params(SCENE_QUERY_STAT(AttributeChanger), false, this);
		const bool bSuccess = GetWorld()->OverlapMultiByChannel(OverlapResults, Start, FQuat::Identity, SMCollisionTraceChannel::Action, CollisionShape, Params);

		if (bSuccess)
		{
			for (const auto& OverlapResult : OverlapResults)
			{
				AActor* ActorToApplyItem = OverlapResult.GetActor();
				if (!ActorToApplyItem)
				{
					continue;
				}

				Result.AddUnique(ActorToApplyItem);
			}
		}

		DrawDebugBox(GetWorld(), Start, CollisionHalfExtend, FColor::Green, false, 0.1f);
	}

	return Result;
}

bool ASMCatchableItem_AttributeChanger::IsValidActorToApplyItem(AActor* TargetActor)
{
	if (!TargetActor)
	{
		return false;
	}

	ISMTeamInterface* TargetTeamInterface = Cast<ISMTeamInterface>(TargetActor);
	if (!ensureAlways(TargetTeamInterface))
	{
		return false;
	}

	// 팀이 같은 경우만 적용됩니다. 추후 변경될 수 있습니다.
	ESMTeam SourceTeam = TeamComponent->GetTeam();
	ESMTeam TargetTeam = TargetTeamInterface->GetTeam();
	if (SourceTeam != TargetTeam)
	{
		return false;
	}

	return true;
}
