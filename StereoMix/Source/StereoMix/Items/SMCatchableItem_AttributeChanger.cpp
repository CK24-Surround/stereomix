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

void ASMCatchableItem_AttributeChanger::ActivateItem(AActor* Activator)
{
	ISMTeamInterface* InstigatorTeamInterface = Cast<ISMTeamInterface>(Activator);
	if (!ensureAlways(InstigatorTeamInterface))
	{
		return;
	}

	ESMTeam SourceTeam = InstigatorTeamInterface->GetTeam();
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
		ApplyItem();

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::TriggerCountTimerCallback, Interval, false);
	}
}

void ASMCatchableItem_AttributeChanger::ApplyItem()
{
	TArray<TWeakObjectPtr<AActor>> ActorsOnTriggeredTiles = GetActorsOnTriggeredTiles(SMCollisionTraceChannel::Action);

	TArray<TWeakObjectPtr<AActor>> ConfirmedActorsToApplyItem;
	for (const auto& ActorOnTriggeredTiles : ActorsOnTriggeredTiles)
	{
		if (!ActorOnTriggeredTiles.Get())
		{
			continue;
		}

		if (IsValidActorToApplyItem(ActorOnTriggeredTiles.Get()))
		{
			// 최종적으로 아이템이 적용될 수 있는 액터만 저장해둡니다.
			ConfirmedActorsToApplyItem.Add(ActorOnTriggeredTiles);
		}
	}

	NET_LOG(this, Warning, TEXT("아이템이 적용될 캐릭터 리스트"));
	for (const auto& ConfirmedActorToApplyItem : ConfirmedActorsToApplyItem)
	{
		NET_LOG(this, Warning, TEXT("%s"), *ConfirmedActorToApplyItem->GetName());
	}
}

TArray<TWeakObjectPtr<AActor>> ASMCatchableItem_AttributeChanger::GetActorsOnTriggeredTiles(ECollisionChannel TraceChannel)
{
	TArray<TWeakObjectPtr<AActor>> Result;
	for (const auto& TriggeredTile : TriggeredTiles)
	{
		if (!ensureAlways(TriggeredTile.Get()))
		{
			continue;
		}

		TArray<FOverlapResult> OverlapResults;
		const FVector TileLocation = TriggeredTile->GetTileLocation();
		FVector Start = TileLocation;
		UBoxComponent* TileBoxComponent = TriggeredTile->GetBoxComponent();
		float HalfTileHorizonSize = TileBoxComponent->GetScaledBoxExtent().X;

		FVector CollisionHalfExtend;
		if (HalfTileHorizonSize > 0.0f)
		{
			const float Height = 50.0f;
			CollisionHalfExtend = FVector(HalfTileHorizonSize, HalfTileHorizonSize, Height);
			Start.Z += Height;
		}

		const FCollisionShape CollisionShape = FCollisionShape::MakeBox(CollisionHalfExtend);
		const FCollisionQueryParams Params(SCENE_QUERY_STAT(AttributeChanger), false, this);
		const bool bSuccess = GetWorld()->OverlapMultiByChannel(OverlapResults, Start, FQuat::Identity, TraceChannel, CollisionShape, Params);
		if (bSuccess)
		{
			for (const auto& OverlapResult : OverlapResults)
			{
				AActor* ActorOnTriggeredTile = OverlapResult.GetActor();
				if (!ActorOnTriggeredTile)
				{
					continue;
				}

				// 트리거된 타일 위에 있는 액터를 중복되지 않게 저장합니다.
				Result.AddUnique(ActorOnTriggeredTile);
			}
		}

		// TODO: 이펙트를 활성화할 곳입니다.

		const FColor DebugColor = bSuccess ? FColor::Cyan : FColor::Green;
		DrawDebugBox(GetWorld(), Start, CollisionHalfExtend, DebugColor, false, Interval / 2);
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
