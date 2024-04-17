// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Utilities/SMTeam.h"
#include "SMCharacterAssetData.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMCharacterAssetData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	USMCharacterAssetData()
	{
		CharacterMaterial.Add(ESMTeam::None, nullptr);
		CharacterMaterial.Add(ESMTeam::EDM, nullptr);
		CharacterMaterial.Add(ESMTeam::FutureBass, nullptr);
	}

public:
	UPROPERTY(EditAnywhere, Category = "Material")
	TMap<ESMTeam, TObjectPtr<UMaterialInstance>> CharacterMaterial;
};
