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

	/** 해당 위치를 기준으로 박스 콜라이더를 이용해 감지된 타일들을 반환합니다. 위치는 타일 중심으로 보정됩니다. BoxExtend의 Z값은 무시 됩니다. */
	static TArray<ASMTile*> GetTilesInBox(const UWorld* World, const FVector& CenterLocation, const FVector& BoxExtend);

	/** 시작 지점에서 끝지점을 향하는 캡슐에 감지된 타일들을 가져옵니다. 시작과 끝 위치는 타일 중심으로 보정됩니다. */
	static TArray<ASMTile*> GetTilesInCapsule(const UWorld* World, const FVector& StartLocation, const FVector& EndLocation, float Radius, bool bShowDebug = false);

	/**
	 * 지정된 타일들을 점령합니다.
	 * @param TilesToBeCaptured 점령할 타일들의 배열입니다.
	 * @param OverrideTeamOption 선택적 팀 매개변수입니다. 제공된다면 Instigator의 팀을 무시하고 이 팀으로 점령됩니다.
	 */
	static void CaptureTiles(const UWorld* World, const TArray<ASMTile*>& TilesToBeCaptured, const AActor* Instigator, const TOptional<ESMTeam>& OverrideTeamOption = TOptional<ESMTeam>());

	/**
	 * 해당 위치를 기준으로 타일을 사각형 모양으로 즉시 점령합니다.
	 * @param TileExpansionCount 중심을 기준으로 점령되는 사이즈입니다.
	 * @param OverrideTeamOption 기본적으로 Instigator의 팀으로 점령이 수행되지만 다른 팀으로 덮어 씌워 수행해야하는 경우 여기에 값을 넣어주면 됩니다. (예시: 타일 중립화 ESMTeam::None)
	 */
	static void CaptureTilesInSquare(const UWorld* World, const FVector& CenterLocation, const AActor* Instigator, int32 TileExpansionCount, const TOptional<ESMTeam>& OverrideTeamOption = TOptional<ESMTeam>());

	/**
	 * 해당 위치를 기준으로 타일을 사각형 모양으로 순차적으로 점령합니다.
	 * @param TileExpansionCount 중심을 기준으로 점령되는 사이즈입니다.
	 * @param OverrideTeamOption 기본적으로 Instigator의 팀으로 점령이 수행되지만 다른 팀으로 덮어 씌워 수행해야하는 경우 여기에 값을 넣어주면 됩니다. (예시: 타일 중립화 ESMTeam::None)
	 */
	static void CaptureTilesInSquareWithDelay(const UWorld* World, const FVector& CenterLocation, const AActor* Instigator, int32 TileExpansionCount, float TotalCaptureTime, const TOptional<ESMTeam>& OverrideTeamOption = TOptional<ESMTeam>());

	static TArray<TWeakObjectPtr<ASMTile>> ConvertToWeakPtrArray(const TArray<ASMTile*>& TilesToBeCaptured);

	/** 기본적으로 사용되는 타일의 사이즈를 나타냅니다. */
	static constexpr float DefaultTileSize = 150.0f;
};
