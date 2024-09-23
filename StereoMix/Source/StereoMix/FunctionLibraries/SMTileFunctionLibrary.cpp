// Copyright Studio Surround. All Rights Reserved.


#include "SMTileFunctionLibrary.h"

#include "Engine/OverlapResult.h"
#include "Games/SMGameState.h"
#include "Tiles/SMTile.h"
#include "Tiles/SMTileManagerComponent.h"
#include "Utilities/SMCollision.h"

USMTileManagerComponent* USMTileFunctionLibrary::GetTileManagerComponent(UWorld* World)
{
	ASMGameState* GameState = World->GetGameState<ASMGameState>();
	if (!World || !GameState)
	{
		return nullptr;
	}

	return GameState->GetTileManager();
}

ASMTile* USMTileFunctionLibrary::GetTileFromLocation(UWorld* World, const FVector& Location)
{
	FHitResult HitResult;
	const FVector StartLocation = Location;
	const FVector EndLocation = StartLocation + (FVector::DownVector * 1000.0);
	if (!World->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, SMCollisionTraceChannel::TileAction))
	{
		return nullptr;
	}

	return Cast<ASMTile>(HitResult.GetActor());
}

TArray<ASMTile*> USMTileFunctionLibrary::GetTilesFromLocationSphere(UWorld* World, const FVector& Location, float Radius)
{
	TArray<ASMTile*> Result;

	TArray<FOverlapResult> OverlapResults;
	const FVector StartLocation = Location;
	const FCollisionShape SphereCollider = FCollisionShape::MakeSphere(Radius);
	if (World->OverlapMultiByChannel(OverlapResults, StartLocation, FQuat::Identity, SMCollisionTraceChannel::TileAction, SphereCollider))
	{
		for (const FOverlapResult& OverlapResult : OverlapResults)
		{
			ASMTile* Tile = Cast<ASMTile>(OverlapResult.GetActor());
			if (Tile)
			{
				Result.Add(Tile);
			}
		}
	}

	return Result;
}

void USMTileFunctionLibrary::TileCaptureImmediateSqaure(UWorld* World, const FVector& StartLocation, ESMTeam InstigatorTeam, int32 CaptureSizeByTile)
{
	USMTileManagerComponent* TileManager = GetTileManagerComponent(World);
	ASMTile* StartTile = GetTileFromLocation(World, StartLocation);
	if (!TileManager || !StartTile)
	{
		return;
	}

	const float Offset = 1.0f;
	const float HalfSize = Offset + (DefaultTileSize * (CaptureSizeByTile - 1));
	TileManager->TileCapture(StartTile, InstigatorTeam, HalfSize, HalfSize);
}

void USMTileFunctionLibrary::TileCaptureImmediateSqaure(UWorld* World, ASMTile* StartTile, ESMTeam InstigatorTeam, int32 CaptureSizeByTile)
{
	USMTileManagerComponent* TileManager = GetTileManagerComponent(World);
	if (!TileManager || !StartTile)
	{
		return;
	}

	const float Offset = 1.0f;
	const float HalfSize = Offset + (DefaultTileSize * (CaptureSizeByTile - 1));
	TileManager->TileCapture(StartTile, InstigatorTeam, HalfSize, HalfSize);
}

void USMTileFunctionLibrary::TileCaptureDelayedSqaure(UWorld* World, const FVector& StartLocation, ESMTeam InstigatorTeam, int32 CaptureSizeByTile, float TotalTime)
{
	USMTileManagerComponent* TileManager = GetTileManagerComponent(World);
	ASMTile* StartTile = GetTileFromLocation(World, StartLocation);
	if (!TileManager || !StartTile)
	{
		return;
	}

	float OffSet = 1.0f;
	const float DeltaTime = TotalTime / (CaptureSizeByTile - 1);

	TWeakObjectPtr<USMTileManagerComponent> TileManagerWeakPtr(TileManager);
	for (int32 i = 0; i < CaptureSizeByTile; ++i)
	{
		float HalfSize = OffSet + DefaultTileSize * i;
		auto Lambda = [TileManagerWeakPtr, StartTile, InstigatorTeam, HalfSize]() {
			if (TileManagerWeakPtr.Get())
			{
				TileManagerWeakPtr->TileCapture(StartTile, InstigatorTeam, HalfSize, HalfSize);
			}
		};

		if (FMath::IsNearlyZero(DeltaTime * i))
		{
			Lambda();
		}
		else
		{
			FTimerHandle TimerHandle;
			World->GetTimerManager().SetTimer(TimerHandle, Lambda, DeltaTime * i, false);
		}
	}
}
