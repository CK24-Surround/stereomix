// Copyright Surround, Inc. All Rights Reserved.


#include "SMTileManagerComponent.h"

#include "Engine/OverlapResult.h"
#include "SMTile.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"


USMTileManagerComponent::USMTileManagerComponent()
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = false;
}

int32 USMTileManagerComponent::TileCapture(ASMTile* StartTile, ESMTeam InstigatorTeam, float HalfHorizenSize, float HalfVerticalSize)
{
	TArray<FOverlapResult> OverlapResults;
	const FVector StartLocation = StartTile->GetTileLocation();
	const FVector HalfExtend(FVector(HalfHorizenSize, HalfVerticalSize, 50.0));
	const FCollisionShape BoxCollision = FCollisionShape::MakeBox(HalfExtend);
	const bool bSuccess = GetWorld()->OverlapMultiByChannel(OverlapResults, StartLocation, FQuat::Identity, SMCollisionTraceChannel::TileAction, BoxCollision);

	int32 SuccessCaptureTileCount = 0;
	if (bSuccess)
	{
		for (const FOverlapResult& OverlapResult : OverlapResults)
		{
			ASMTile* Tile = Cast<ASMTile>(OverlapResult.GetActor());
			if (!Tile)
			{
				continue;
			}

			const ESMTeam TileTeam = Tile->GetTeam();
			if (InstigatorTeam == TileTeam)
			{
				continue;
			}

			Tile->TileTrigger(InstigatorTeam);
			++SuccessCaptureTileCount;
		}
	}

	if (bShowDebug)
	{
		DrawDebugBox(GetWorld(), StartLocation, HalfExtend, FColor::Turquoise, false, 2.0f);
	}

	return SuccessCaptureTileCount;
}
