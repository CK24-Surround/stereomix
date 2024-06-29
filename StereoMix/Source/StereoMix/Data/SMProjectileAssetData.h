// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SMTeam.h"

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

	UPROPERTY(EditAnywhere, Category = "Material")
	TMap<ESMTeam, TObjectPtr<UMaterialInstance>> ProjectileMaterial;
};
