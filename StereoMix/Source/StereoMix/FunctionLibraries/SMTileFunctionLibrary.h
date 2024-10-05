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
	static USMTileManagerComponent* GetTileManagerComponent(const UWorld* World);

	/** 해당 위치에서 가장 가까운 타일을 가져옵니다. */
	static ASMTile* GetTileFromLocation(const UWorld* World, const FVector& Location);

	/** 시작 지점에서 끝지점을 향하는 캡슐에 감지된 타일들을 가져옵니다. */
	static TArray<ASMTile*> GetTilesFromLocationByCapsule(const UWorld* World, const FVector& StartLocation, const FVector& EndLocaiton, float Radius, bool bShowDebug = false);

	/**
	 * 해당 위치를 기준으로 타일을 사각형 모양으로 즉시 점령합니다.
	 * @param TileExpansionCount 중심을 기준으로 점령되는 사이즈입니다.
	 * @param OverrideTeamOption 기본적으로 Instigator의 팀으로 점령이 수행되지만 다른 팀으로 덮어 씌워 수행해야하는 경우 여기에 값을 넣어주면 됩니다. (예시: 타일 중립화 ESMTeam::None)
	 */
	static void CaptureTilesInSqaure(const UWorld* World, const FVector& StartLocation, const AActor* Instigator, int32 TileExpansionCount, const TOptional<ESMTeam>& OverrideTeamOption = TOptional<ESMTeam>());

	/**
	 * 해당 위치를 기준으로 타일을 사각형 모양으로 즉시 점령합니다.
	 * @param TileExpansionCount 중심을 기준으로 점령되는 사이즈입니다.
	 * @param OverrideTeamOption 기본적으로 Instigator의 팀으로 점령이 수행되지만 다른 팀으로 덮어 씌워 수행해야하는 경우 여기에 값을 넣어주면 됩니다. (예시: 타일 중립화 ESMTeam::None)
	 */
	static void CaptureTilesInSqaure(const UWorld* World, ASMTile* StartTile, const AActor* Instigator, int32 TileExpansionCount, const TOptional<ESMTeam>& OverrideTeamOption = TOptional<ESMTeam>());

	/**
	 * 해당 위치를 기준으로 타일을 사각형 모양으로 순차적으로 점령합니다.
	 * @param TileExpansionCount 중심을 기준으로 점령되는 사이즈입니다.
	 * @param OverrideTeamOption 기본적으로 Instigator의 팀으로 점령이 수행되지만 다른 팀으로 덮어 씌워 수행해야하는 경우 여기에 값을 넣어주면 됩니다. (예시: 타일 중립화 ESMTeam::None)
	 */
	static void CaptureTilesInSqaureWithDelay(const UWorld* World, const FVector& StartLocation, const AActor* Instigator, int32 TileExpansionCount, float TotalCaptureTime, const TOptional<ESMTeam>& OverrideTeamOption = TOptional<ESMTeam>());

	/** 기본적으로 사용되는 타일의 사이즈를 나타냅니다. */
	static constexpr float DefaultTileSize = 150.0f;
};
