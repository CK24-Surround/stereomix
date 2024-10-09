// Copyright Studio Surround. All Rights Reserved.


#include "SMTileFunctionLibrary.h"

#include "Engine/OverlapResult.h"
#include "GameFramework/GameStateBase.h"
#include "Actors/Tiles/SMTile.h"
#include "Components/Core/SMTileManagerComponent.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"

USMTileManagerComponent* USMTileFunctionLibrary::GetTileManagerComponent(const UWorld* World)
{
	const AGameStateBase* GameState = World ? World->GetGameState() : nullptr;
	return GameState ? GameState->GetComponentByClass<USMTileManagerComponent>() : nullptr;
}

ASMTile* USMTileFunctionLibrary::GetTileFromLocation(const UWorld* World, const FVector& Location)
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

TArray<ASMTile*> USMTileFunctionLibrary::GetTilesInBox(const UWorld* World, const FVector& CenterLocation, const FVector& BoxExtend)
{
	TArray<ASMTile*> Result;

	const ASMTile* CenterTile = GetTileFromLocation(World, CenterLocation);
	if (!CenterTile)
	{
		return Result;
	}

	TArray<FOverlapResult> OverlapResults;
	const FVector StartLocation = CenterTile->GetTileLocation();
	const FCollisionShape BoxCollision = FCollisionShape::MakeBox(FVector(BoxExtend.X, BoxExtend.Y, 50.0));
	if (!World->OverlapMultiByChannel(OverlapResults, StartLocation, FQuat::Identity, SMCollisionTraceChannel::TileAction, BoxCollision))
	{
		return Result;
	}

	for (const FOverlapResult& OverlapResult : OverlapResults)
	{
		if (ASMTile* Tile = Cast<ASMTile>(OverlapResult.GetActor()))
		{
			Result.Add(Tile);
		}
	}

	return Result;
}

TArray<ASMTile*> USMTileFunctionLibrary::GetTilesInCapsule(const UWorld* World, const FVector& StartLocation, const FVector& EndLocation, float Radius, bool bShowDebug)
{
	TArray<ASMTile*> Result;

	TArray<FOverlapResult> OverlapResults;
	const FVector CapsuleLocation = (StartLocation + EndLocation) / 2.0f;
	const FVector CapsuleDirection = (EndLocation - StartLocation).GetSafeNormal();
	const FRotator CapsuleRotation = CapsuleDirection.Rotation() + FRotator(90.0, 0.0, 0.0);
	const FQuat CapsuleQuat = CapsuleRotation.Quaternion();
	const float CapsuleHalfHeight = (FVector::Dist(StartLocation, EndLocation) / 2.0f) + Radius;
	const FCollisionShape CapsuleCollider = FCollisionShape::MakeCapsule(Radius, CapsuleHalfHeight);
	if (World->OverlapMultiByChannel(OverlapResults, CapsuleLocation, CapsuleQuat, SMCollisionTraceChannel::TileAction, CapsuleCollider))
	{
		for (const FOverlapResult& OverlapResult : OverlapResults)
		{
			if (ASMTile* Tile = Cast<ASMTile>(OverlapResult.GetActor()))
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

void USMTileFunctionLibrary::CaptureTiles(const UWorld* World, const TArray<ASMTile*>& TilesToBeCaptured, const AActor* Instigator, const TOptional<ESMTeam>& OverrideTeamOption)
{
	if (USMTileManagerComponent* TileManager = GetTileManagerComponent(World))
	{
		TileManager->CaptureTiles(TilesToBeCaptured, Instigator, OverrideTeamOption);
	}
}

void USMTileFunctionLibrary::CaptureTilesInSquare(const UWorld* World, const FVector& CenterLocation, const AActor* Instigator, int32 TileExpansionCount, const TOptional<ESMTeam>& OverrideTeamOption)
{
	USMTileManagerComponent* TileManager = GetTileManagerComponent(World);
	const ASMTile* CenterTile = GetTileFromLocation(World, CenterLocation);
	if (!TileManager || !CenterTile)
	{
		return;
	}

	constexpr float Offset = DefaultTileSize / 4.0f;
	const float HalfSize = Offset + (DefaultTileSize * (TileExpansionCount - 1));
	const FVector BoxExtend(HalfSize);
	TileManager->CaptureTiles(GetTilesInBox(World, CenterLocation, BoxExtend), Instigator, OverrideTeamOption);
}

void USMTileFunctionLibrary::CaptureTilesInSquareWithDelay(const UWorld* World, const FVector& CenterLocation, const AActor* Instigator, int32 TileExpansionCount, float TotalCaptureTime, const TOptional<ESMTeam>& OverrideTeamOption)
{
	USMTileManagerComponent* TileManager = GetTileManagerComponent(World);
	const ASMTile* CenterTile = GetTileFromLocation(World, CenterLocation);
	if (!TileManager || !CenterTile)
	{
		return;
	}

	constexpr float OffSet = DefaultTileSize / 4.0f;
	const float StepTime = TotalCaptureTime / (TileExpansionCount - 1);

	TWeakObjectPtr<USMTileManagerComponent> TileManagerWeakPtr(TileManager);
	for (int32 i = 0; i < TileExpansionCount; ++i)
	{
		const float HalfSize = OffSet + (DefaultTileSize * i);
		TArray<ASMTile*> Tiles = GetTilesInBox(World, CenterLocation, FVector(HalfSize));

		auto CaptureTilesByStep = [TileManagerWeakPtr, Tiles, Instigator, OverrideTeamOption]() {
			if (TileManagerWeakPtr.IsValid())
			{
				TileManagerWeakPtr->CaptureTiles(Tiles, Instigator, OverrideTeamOption);
			}
		};

		if (FMath::IsNearlyZero(StepTime * i))
		{
			CaptureTilesByStep();
		}
		else
		{
			FTimerHandle TimerHandle;
			World->GetTimerManager().SetTimer(TimerHandle, CaptureTilesByStep, StepTime * i, false);
		}
	}
}

TArray<TWeakObjectPtr<ASMTile>> USMTileFunctionLibrary::ConvertToWeakPtrArray(const TArray<ASMTile*>& TilesToBeCaptured)
{
	TArray<TWeakObjectPtr<ASMTile>> Result;
	for (ASMTile* TileToBeCaptured : TilesToBeCaptured)
	{
		Result.Add(TileToBeCaptured);
	}

	return Result;
}
