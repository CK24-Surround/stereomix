// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SMProjectileAssetData.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMProjectileAssetData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	USMProjectileAssetData()
	{
		ProjectileMaterial.Add(ESMTeam::None, nullptr);
		ProjectileMaterial.Add(ESMTeam::EDM, nullptr);
		ProjectileMaterial.Add(ESMTeam::FutureBass, nullptr);
	}

public:
	UPROPERTY(EditAnywhere, Category = "Material")
	TMap<ESMTeam, TObjectPtr<UMaterialInstance>> ProjectileMaterial;
};
