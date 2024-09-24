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
	/** 타일 매니저 컴포넌트를 가져옵니다.*/
	static USMTileManagerComponent* GetTileManagerComponent(UWorld* World);

	/** 해당 위치에서 가장 가까운 타일을 가져옵니다. */
	static ASMTile* GetTileFromLocation(UWorld* World, const FVector& Location);

	/** 시작 지점에서 끝지점을 향하는 캡슐에 감지된 타일들을 가져옵니다. */
	static TArray<ASMTile*> GetTilesFromLocationByCapsule(UWorld* World, const FVector& StartLocation, const FVector& EndLocaiton, float Radius, bool bShowDebug = false);

	/**
	 * 해당 위치를 기준으로 타일을 사각형 모양으로 즉시 점령합니다.
	 * @param CaptureSizeByTile 중심을 기준으로 점령되는 사이즈입니다.
	 */
	static void TileCaptureImmediateSqaure(UWorld* World, const FVector& StartLocation, ESMTeam InstigatorTeam, int32 CaptureSizeByTile, bool bShowDebug = false);

	/**
	 * 해당 타일을 기준으로 타일을 사각형 모양으로 즉시 점령합니다.
	 * @param CaptureSizeByTile 중심을 기준으로 점령되는 사이즈입니다.
	 */
	static void TileCaptureImmediateSqaure(UWorld* World, ASMTile* StartTile, ESMTeam InstigatorTeam, int32 CaptureSizeByTile, bool bShowDebug = false);

	/**
	 * 해당 위치를 기준으로 타일을 사각형 모양으로 순차적으로 점령합니다.
	 * @param CaptureSizeByTile 중심을 기준으로 점령되는 사이즈입니다.
	 */
	static void TileCaptureDelayedSqaure(UWorld* World, const FVector& StartLocation, ESMTeam InstigatorTeam, int32 CaptureSizeByTile, float TotalTime, bool bShowDebug = false);

	/** 기본적으로 사용되는 타일의 사이즈를 나타냅니다. */
	static constexpr float DefaultTileSize = 150.0f;
};
