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

void USMTileManagerComponent::TileCaptureImmediateSqaure(ASMTile* StartTile, ESMTeam InstigatorTeam, int32 CaptureSizeByTile)
{
	const float HalfSize = (CaptureSizeByTile * TileHorizenSize) / 2.0f;
	TileCapture(StartTile, InstigatorTeam, HalfSize, HalfSize);
}

void USMTileManagerComponent::TileCaptureDelayedSqaure(ASMTile* StartTile, ESMTeam InstigatorTeam, int32 CaptureSizeByTile, float TotalTime)
{
	NET_LOG(GetOwner(), Warning, TEXT("타일 트리거!"));

	float OffSet = 1.0f;
	const float DeltaTime = TotalTime / (CaptureSizeByTile - 1);

	TWeakObjectPtr<USMTileManagerComponent> WeakThis(this);
	for (int32 i = 0; i < CaptureSizeByTile; ++i)
	{ 
		float HalfSize = OffSet + TileHorizenSize * i;
		auto Lambda = [WeakThis, StartTile, InstigatorTeam, HalfSize]() {
			if (WeakThis.Get())
			{
				WeakThis->TileCapture(StartTile, InstigatorTeam, HalfSize, HalfSize);
			}
		};

		if (FMath::IsNearlyZero(DeltaTime * i))
		{
			Lambda();
		}
		else
		{
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, Lambda, DeltaTime * i, false);
		}
	}
}

void USMTileManagerComponent::TileCapture(ASMTile* StartTile, ESMTeam InstigatorTeam, float HalfHorizenSize, float HalfVerticalSize)
{
	TArray<FOverlapResult> OverlapResults;
	const FVector StartLocation = StartTile->GetTileLocation();
	const FVector HalfExtend(FVector(HalfHorizenSize, HalfVerticalSize, 50.0));
	const FCollisionShape BoxCollision = FCollisionShape::MakeBox(HalfExtend);
	const bool bSuccess = GetWorld()->OverlapMultiByChannel(OverlapResults, StartLocation, FQuat::Identity, SMCollisionTraceChannel::TileAction, BoxCollision);

	if (bSuccess)
	{
		for (const FOverlapResult& OverlapResult : OverlapResults)
		{
			ASMTile* Tile = Cast<ASMTile>(OverlapResult.GetActor());
			if (Tile)
			{
				Tile->TileTrigger(InstigatorTeam);
			}
		}
	}

	if (bShowDebug)
	{
		DrawDebugBox(GetWorld(), StartLocation, HalfExtend, FColor::Turquoise, false, 2.0f);
	}
}
