// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMAICharacterBase.h"
#include "Data/DataTable/SMCharacterData.h"
#include "SMAIElectricGuitarCharacter.generated.h"

UCLASS()
class STEREOMIX_API ASMAIElectricGuitarCharacter : public ASMAICharacterBase
{
	GENERATED_BODY()

public:
	ASMAIElectricGuitarCharacter();

	virtual void Attack(AActor* AttackTarget) override;

	float Damage = 0.0f;

	float MaxDistanceByTile = 0.0f;

	float ProjectileSpeed = 0.0f;

	float SpreadAngle = 0.0f;

	int32 AccuracyShootRate = 0;
};
