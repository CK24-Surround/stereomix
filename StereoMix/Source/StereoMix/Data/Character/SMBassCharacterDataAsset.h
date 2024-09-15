// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMPlayerCharacterDataAsset.h"
#include "SMBassCharacterDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMBassCharacterDataAsset : public USMPlayerCharacterDataAsset
{
	GENERATED_BODY()

public:
	USMBassCharacterDataAsset()
	{
		WeaponSocketName = TEXT("ik_hand_weapon_r");
	}
};
