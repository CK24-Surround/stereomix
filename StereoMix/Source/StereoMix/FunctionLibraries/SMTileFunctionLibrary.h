// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMFunctionLibraryBase.h"
#include "Data/SMTeam.h"
#include "SMTileFunctionLibrary.generated.h"

class ASMTile;
class USMTileManagerComponent;

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMTileFunctionLibrary : public USMFunctionLibraryBase
{
	GENERATED_BODY()

public:
	static USMTileManagerComponent* GetTileManagerComponent(UWorld* World);

	static ASMTile* GetTileFromLocation(UWorld* World, const FVector& Location);

	static TArray<ASMTile*> GetTilesFromLocationSphere(UWorld* World, const FVector& Location, float Radius);

	static void TileCaptureImmediateSqaure(UWorld* World, const FVector& StartLocation, ESMTeam InstigatorTeam, int32 CaptureSizeByTile);

	static void TileCaptureImmediateSqaure(UWorld* World, ASMTile* StartTile, ESMTeam InstigatorTeam, int32 CaptureSizeByTile);

	static void TileCaptureDelayedSqaure(UWorld* World, const FVector& StartLocation, ESMTeam InstigatorTeam, int32 CaptureSizeByTile, float TotalTime);

	static constexpr float DefaultTileSize = 150.0f;
};
