// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Data/SMTeam.h"
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
		CharacterStateWidget.Add(ESMTeam::None, nullptr);
		CharacterStateWidget.Add(ESMTeam::EDM, nullptr);
		CharacterStateWidget.Add(ESMTeam::FutureBass, nullptr);
		
		HairFrontMaterial.Add(ESMTeam::None, nullptr);
		HairFrontMaterial.Add(ESMTeam::EDM, nullptr);
		HairFrontMaterial.Add(ESMTeam::FutureBass, nullptr);

		HairRingMaterial.Add(ESMTeam::None, nullptr);
		HairRingMaterial.Add(ESMTeam::EDM, nullptr);
		HairRingMaterial.Add(ESMTeam::FutureBass, nullptr);

		HairTailMaterial.Add(ESMTeam::None, nullptr);
		HairTailMaterial.Add(ESMTeam::EDM, nullptr);
		HairTailMaterial.Add(ESMTeam::FutureBass, nullptr);
	}

public:
	UPROPERTY(EditAnywhere, Category = "Widget")
	TMap<ESMTeam, TSubclassOf<UUserWidget>> CharacterStateWidget;

	UPROPERTY(EditAnywhere, Category = "Material")
	TMap<ESMTeam, TObjectPtr<UMaterialInstance>> HairFrontMaterial;

	UPROPERTY(EditAnywhere, Category = "Material")
	TMap<ESMTeam, TObjectPtr<UMaterialInterface>> HairRingMaterial;

	UPROPERTY(EditAnywhere, Category = "Material")
	TMap<ESMTeam, TObjectPtr<UMaterialInterface>> HairTailMaterial;
};
