// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMAICharacterBase.h"
#include "SMAIElectricGuitarCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAICharacterAttack);

UCLASS()
class STEREOMIX_API ASMAIElectricGuitarCharacter : public ASMAICharacterBase
{
	GENERATED_BODY()

public:
	ASMAIElectricGuitarCharacter();

	virtual void BeginPlay() override;

	virtual void Attack(AActor* AttackTarget) override;

	float Damage = 0.0f;

	float MaxDistanceByTile = 0.0f;

	float ProjectileSpeed = 0.0f;

	float SpreadAngle = 0.0f;

	int32 AccuracyShootRate = 0;

	UPROPERTY(BlueprintAssignable)
	FOnAICharacterAttack OnAttack;
};
