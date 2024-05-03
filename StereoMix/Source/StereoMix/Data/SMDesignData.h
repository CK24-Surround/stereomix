// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SMDesignData.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMDesignData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "PlayerCharacter|Movement", DisplayName = "이동속도")
	float MoveSpeed = 700.0f;
};
