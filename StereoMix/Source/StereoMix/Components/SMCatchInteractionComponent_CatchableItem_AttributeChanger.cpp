// Copyright Surround, Inc. All Rights Reserved.


#include "SMCatchInteractionComponent_CatchableItem_AttributeChanger.h"

#include "Components/BoxComponent.h"
#include "Engine/OverlapResult.h"
#include "Items/SMCatchableItem_AttributeChanger.h"
#include "Tiles/SMTile.h"
#include "Utilities/SMCollision.h"


USMCatchInteractionComponent_CatchableItem_AttributeChanger::USMCatchInteractionComponent_CatchableItem_AttributeChanger() {}

void USMCatchInteractionComponent_CatchableItem_AttributeChanger::OnSpecialActionPerformed(AActor* Instigator, ESpecialAction InSpecialAction) {}

void USMCatchInteractionComponent_CatchableItem_AttributeChanger::OnSpecialActionEnded(AActor* Instigator, ESpecialAction InSpecialAction, float InMagnitude, TSubclassOf<UGameplayEffect> DamageGE, float DamageAmount)
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		return;
	}

	switch (InSpecialAction)
	{
		case ESpecialAction::Smash:
		{
			InternalOnSmashedEnded(Instigator, InMagnitude);
			break;
		}
	}
}

void USMCatchInteractionComponent_CatchableItem_AttributeChanger::InternalOnSmashedEnded(AActor* Instigator, float InMagnitude)
{
	ASMCatchableItem_AttributeChanger* SourceItem = Cast<ASMCatchableItem_AttributeChanger>(SourceActor);
	if (!ensureAlways(SourceItem))
	{
		return;
	}

	// 아이템을 놓아줍니다.
	InternalOnCaughtReleased(Instigator);

	// 아이템의 바로아래 타일을 트리거하고 이 타일을 기준으로 스매시의 크기만큼의 타일을 골라내 소스액터에 저장합니다.
	ASMTile* TriggeredTile = TileTrigger();
	if (TriggeredTile)
	{
		SaveTriggeredTileLocationsBySmash(TriggeredTile, InMagnitude);
	}

	// 아이템을 활성화합니다.
	ISMTeamInterface* InstigatorTeamInterface = Cast<ISMTeamInterface>(Instigator);
	if (InstigatorTeamInterface)
	{
		SourceItem->ActivateItem(InstigatorTeamInterface->GetTeam());
	}
}

ASMTile* USMCatchInteractionComponent_CatchableItem_AttributeChanger::TileTrigger()
{
	FHitResult HitResult;
	const FVector Start = SourceActor->GetActorLocation();
	const FVector End = Start + (-FVector::UpVector * 1000.0f);
	const FCollisionQueryParams Param(SCENE_QUERY_STAT(TileTrace), false);
	const FCollisionShape CollisionShape = FCollisionShape::MakeSphere(25.0f);
	const bool bSuccess = GetWorld()->SweepSingleByChannel(HitResult, Start, End, FQuat::Identity, SMCollisionTraceChannel::TileAction, CollisionShape, Param);
	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 3.0f);

	if (bSuccess)
	{
		ASMTile* TriggeredTile = Cast<ASMTile>(HitResult.GetActor());
		if (TriggeredTile)
		{
			return TriggeredTile;
		}
	}

	return nullptr;
}

void USMCatchInteractionComponent_CatchableItem_AttributeChanger::SaveTriggeredTileLocationsBySmash(ASMTile* InTriggeredTile, float InMagnitude)
{
	UBoxComponent* TileBoxComponent = InTriggeredTile->GetBoxComponent();
	float TileHorizonSize = TileBoxComponent->GetScaledBoxExtent().X * 2;

	// 아이템이 적용되야할 타일의 위치를 구하기 위한 범위입니다.
	TArray<FOverlapResult> OverlapResults;
	const FVector Start = InTriggeredTile->GetTileLocation();
	float HalfHorizenSize = (TileHorizonSize * (InMagnitude - 1)) + (TileHorizonSize / 2);
	// 정확히 일치해버리면 바깥쪽의 타일까지 트리거되기때문에 약간 작은 크기로 바꿔줘야합니다. 깔끔하게 보이도록 -1.0f 대신 TileHorizonSize를 빼주었습니다.
	HalfHorizenSize -= TileHorizonSize;

	FVector CollisionHalfExtend;
	if (HalfHorizenSize > 0.0f)
	{
		CollisionHalfExtend = FVector(HalfHorizenSize, HalfHorizenSize, 100.0);
	}

	const FCollisionShape CollisionShape = FCollisionShape::MakeBox(CollisionHalfExtend);
	const FCollisionQueryParams Params(SCENE_QUERY_STAT(AttributeChanger), false);
	const bool bSuccess = GetWorld()->OverlapMultiByChannel(OverlapResults, Start, FQuat::Identity, SMCollisionTraceChannel::TileAction, CollisionShape, Params);

	if (bSuccess)
	{
		for (const auto& OverlapResult : OverlapResults)
		{
			ASMTile* TriggeredTile = Cast<ASMTile>(OverlapResult.GetActor());
			if (!ensureAlways(TriggeredTile))
			{
				continue;
			}

			ASMCatchableItem_AttributeChanger* SourceItem = Cast<ASMCatchableItem_AttributeChanger>(SourceActor);
			if (!ensureAlways(SourceItem))
			{
				continue;
			}

			// 트리거된 타일들을 아이템에게 저장해줍니다.
			SourceItem->TriggeredTiles.Add(TriggeredTile);
		}
	}
}
