// Copyright Studio Surround. All Rights Reserved.


#include "SMTileFunctionLibrary.h"

#include "Engine/OverlapResult.h"
#include "Games/SMGameState.h"
#include "Tiles/SMTile.h"
#include "Tiles/SMTileManagerComponent.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"

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
	if (!World)
	{
		return nullptr;
	}

	FHitResult HitResult;
	const FVector StartLocation = Location;
	const FVector EndLocation = StartLocation + (FVector::DownVector * 1000.0);
	World->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, SMCollisionTraceChannel::TileAction);
	return Cast<ASMTile>(HitResult.GetActor());
}

TArray<ASMTile*> USMTileFunctionLibrary::GetTilesFromLocationByCapsule(UWorld* World, const FVector& StartLocation, const FVector& EndLocaiton, float Radius, bool bShowDebug)
{
	TArray<ASMTile*> Result;

	TArray<FOverlapResult> OverlapResults;
	const FVector CapsuleLocation = (StartLocation + EndLocaiton) / 2.0f;
	const FVector CapsuleDirection = (EndLocaiton - StartLocation).GetSafeNormal();
	const FRotator CapsuleRotation = CapsuleDirection.Rotation() + FRotator(90.0, 0.0, 0.0);
	const FQuat CapsuleQuat = CapsuleRotation.Quaternion();
	const float CapsuleHalfHeight = (FVector::Dist(StartLocation, EndLocaiton) / 2.0f) + Radius;
	const FCollisionShape CapsuleCollider = FCollisionShape::MakeCapsule(Radius, CapsuleHalfHeight);
	if (World->OverlapMultiByChannel(OverlapResults, CapsuleLocation, CapsuleQuat, SMCollisionTraceChannel::TileAction, CapsuleCollider))
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

	if (bShowDebug)
	{
		DrawDebugCapsule(World, CapsuleLocation, CapsuleHalfHeight, Radius, CapsuleQuat, FColor::Cyan, false, 2.0f);
	}

	return Result;
}

void USMTileFunctionLibrary::TileCaptureImmediateSqaure(UWorld* World, const FVector& StartLocation, ESMTeam InstigatorTeam, int32 CaptureSizeByTile, bool bShowDebug)
{
	USMTileManagerComponent* TileManager = GetTileManagerComponent(World);
	ASMTile* StartTile = GetTileFromLocation(World, StartLocation);
	if (!TileManager || !StartTile)
	{
		return;
	}

	const float Offset = 1.0f;
	const float HalfSize = Offset + (DefaultTileSize * (CaptureSizeByTile - 1));
	TileManager->TileCapture(StartTile, InstigatorTeam, HalfSize, HalfSize, bShowDebug);
}

void USMTileFunctionLibrary::TileCaptureImmediateSqaure(UWorld* World, ASMTile* StartTile, ESMTeam InstigatorTeam, int32 CaptureSizeByTile, bool bShowDebug)
{
	USMTileManagerComponent* TileManager = GetTileManagerComponent(World);
	if (!TileManager || !StartTile)
	{
		return;
	}

	const float Offset = 1.0f;
	const float HalfSize = Offset + (DefaultTileSize * (CaptureSizeByTile - 1));
	TileManager->TileCapture(StartTile, InstigatorTeam, HalfSize, HalfSize, bShowDebug);
}

void USMTileFunctionLibrary::TileCaptureDelayedSqaure(UWorld* World, const FVector& StartLocation, ESMTeam InstigatorTeam, int32 CaptureSizeByTile, float TotalTime, bool bShowDebug)
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
		auto Lambda = [TileManagerWeakPtr, StartTile, InstigatorTeam, HalfSize, bShowDebug]() {
			if (TileManagerWeakPtr.Get())
			{
				TileManagerWeakPtr->TileCapture(StartTile, InstigatorTeam, HalfSize, HalfSize, bShowDebug);
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
