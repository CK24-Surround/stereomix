// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/SMTeam.h"
#include "SMTileManagerComponent.generated.h"


class ASMTile;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMTileManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USMTileManagerComponent();

	void TileCaptureImmediateSqaure(ASMTile* StartTile, ESMTeam InstigatorTeam, int32 CaptureSizeByTile);

	void TileCaptureDelayedSqaure(ASMTile* StartTile, ESMTeam InstigatorTeam, int32 CaptureSizeByTile, float TotalTime);

protected:
	void TileCapture(ASMTile* StartTile, ESMTeam InstigatorTeam, float HalfHorizenSize, float HalfVerticalSize);

	UPROPERTY(EditAnywhere, Category = "Design")
	float TileHorizenSize = 150.0f;

	UPROPERTY(EditAnywhere, Category = "Design")
	uint32 bShowDebug:1 = false;
};
