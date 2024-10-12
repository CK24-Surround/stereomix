// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMAICharacterBase.h"
#include "SMAIElectricGuitarCharacter.generated.h"

UCLASS()
class STEREOMIX_API ASMAIElectricGuitarCharacter : public ASMAICharacterBase
{
	GENERATED_BODY()

public:
	virtual void Attack(AActor* AttackTarget, float Accuracy, float Damage) override;
};
