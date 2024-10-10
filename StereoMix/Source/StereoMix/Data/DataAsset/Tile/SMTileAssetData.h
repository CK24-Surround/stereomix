// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Data/SMTeam.h"

#include "SMTileAssetData.generated.h"

class UNiagaraSystem;

/**
 *
 */
UCLASS()
class STEREOMIX_API USMTileAssetData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	USMTileAssetData()
	{
		for (int32 i = 1; i < static_cast<int32>(ESMTeam::Max); ++i)
		{
			const ESMTeam Team = static_cast<ESMTeam>(i);
			CapturedTileMaterial.Add(Team, nullptr);
			CapturedTileOverlayMaterial.Add(Team, nullptr);
			TileChangeFX.Add(Team, nullptr);
		}
	}

	UPROPERTY(EditAnywhere, Category = "Material")
	TMap<ESMTeam, TObjectPtr<UMaterialInterface>> CapturedTileMaterial;

	UPROPERTY(EditAnywhere, Category = "Material")
	TMap<ESMTeam, TObjectPtr<UMaterialInterface>> CapturedTileOverlayMaterial;

	UPROPERTY(EditAnywhere, Category = "Material")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> TileChangeFX;
};
