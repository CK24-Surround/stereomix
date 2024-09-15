// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMPlayerCharacterDataAsset.h"
#include "SMPianoCharacterDataAsset.generated.h"

class UInputAction;

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMPianoCharacterDataAsset : public USMPlayerCharacterDataAsset
{
	GENERATED_BODY()

public:
	USMPianoCharacterDataAsset()
	{
		WeaponSocketName = TEXT("ik_hand_weapon_l");
	}

	UPROPERTY(EditAnywhere, Category = "FX")
	TMap<ESMTeam, TObjectPtr<UNiagaraSystem>> ImpactArrowIndicator;

	UPROPERTY(EditAnywhere, Category = "Dummy")
	TObjectPtr<UCurveFloat> NoiseBreakCurve;
};
