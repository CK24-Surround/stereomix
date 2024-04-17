// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Utilities/SMTeam.h"
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

public:
	UPROPERTY(EditAnywhere, Category = "Material")
	TMap<ESMTeam, TObjectPtr<UMaterialInterface>> TileMaterial;

	UPROPERTY(EditAnywhere, Category = "Material")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> TileChangeFX;
};
