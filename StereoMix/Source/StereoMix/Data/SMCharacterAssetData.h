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
		CharacterStateWidget.Add(ESMTeam::None, nullptr);
		CharacterStateWidget.Add(ESMTeam::EDM, nullptr);
		CharacterStateWidget.Add(ESMTeam::FutureBass, nullptr);
		
		CharacterMaterial.Add(ESMTeam::None, nullptr);
		CharacterMaterial.Add(ESMTeam::EDM, nullptr);
		CharacterMaterial.Add(ESMTeam::FutureBass, nullptr);
	}

public:
	UPROPERTY(EditAnywhere, Category = "Widget")
	TMap<ESMTeam, TSubclassOf<UUserWidget>> CharacterStateWidget;

	UPROPERTY(EditAnywhere, Category = "Material")
	TMap<ESMTeam, TObjectPtr<UMaterialInstance>> CharacterMaterial;
};
