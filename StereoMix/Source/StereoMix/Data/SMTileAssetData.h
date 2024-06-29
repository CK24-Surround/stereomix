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
		TileMaterial.Add(ESMTeam::None, nullptr);
		TileMaterial.Add(ESMTeam::EDM, nullptr);
		TileMaterial.Add(ESMTeam::FutureBass, nullptr);

		TileChangeFX.Add(ESMTeam::None, nullptr);
		TileChangeFX.Add(ESMTeam::EDM, nullptr);
		TileChangeFX.Add(ESMTeam::FutureBass, nullptr);
	}

	UPROPERTY(EditAnywhere, Category = "Material")
	TMap<ESMTeam, TObjectPtr<UMaterialInterface>> TileMaterial;

	UPROPERTY(EditAnywhere, Category = "Material")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> TileChangeFX;
};
