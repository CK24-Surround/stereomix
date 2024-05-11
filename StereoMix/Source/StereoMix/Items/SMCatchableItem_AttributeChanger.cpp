// Copyright Surround, Inc. All Rights Reserved.


#include "SMCatchableItem_AttributeChanger.h"

#include "Components/BoxComponent.h"
#include "Components/SMCatchInteractionComponent_CatchableItem_AttributeChanger.h"
#include "Engine/OverlapResult.h"
#include "Tiles/SMTile.h"
#include "Utilities/SMCollision.h"


ASMCatchableItem_AttributeChanger::ASMCatchableItem_AttributeChanger()
{
	CIC = CreateDefaultSubobject<USMCatchInteractionComponent_CatchableItem_AttributeChanger>(TEXT("CIC"));
}

USMCatchInteractionComponent* ASMCatchableItem_AttributeChanger::GetCatchInteractionComponent()
{
	return CIC;
}

void ASMCatchableItem_AttributeChanger::ActivateItem()
{
	HandleApplyItem();
}

void ASMCatchableItem_AttributeChanger::HandleApplyItem()
{
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
		const FCollisionQueryParams Params(SCENE_QUERY_STAT(AttributeChanger), false);
		const bool bSuccess = GetWorld()->OverlapMultiByChannel(OverlapResults, Start, FQuat::Identity, SMCollisionTraceChannel::TileAction, CollisionShape, Params);

		if (bSuccess)
		{
			DrawDebugBox(GetWorld(), Start, CollisionHalfExtend, FColor::Green, false, 3.0f);
		}
	}
}
