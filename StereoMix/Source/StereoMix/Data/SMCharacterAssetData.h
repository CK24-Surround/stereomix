// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SMCharacterAssetData.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMCharacterAssetData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Material")
	TObjectPtr<UMaterialInterface> FutureBassMaterial;

	UPROPERTY(EditAnywhere, Category = "Material")
	TObjectPtr<UMaterialInterface> EDMMaterial;
};
