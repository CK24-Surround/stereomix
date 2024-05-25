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
	UPROPERTY(EditAnywhere, Category = "Widget")
	TMap<ESMTeam, TSubclassOf<UUserWidget>> CharacterStateWidget;
};
