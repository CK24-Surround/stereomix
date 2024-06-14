// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMTeam.h"
#include "Engine/DataAsset.h"
#include "SMCharacterSelectOptionData.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMCharacterSelectOptionData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Default Options")
	ESMTeam DefaultTeam;
};
