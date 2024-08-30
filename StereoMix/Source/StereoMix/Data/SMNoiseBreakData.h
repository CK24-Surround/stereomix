#pragma once

#include "SMNoiseBreakData.generated.h"

class UGameplayEffect;

USTRUCT()
struct FSMNoiseBreakData
{
	GENERATED_BODY()

	virtual ~FSMNoiseBreakData() = default;

	float Magnitude;
};

USTRUCT()
struct FSMNoiseBreakData_Character
{
	GENERATED_BODY()

	TSubclassOf<UGameplayEffect> DamageGE;
	float DamageAmount;
};

USTRUCT()
struct FSMNoiseBreakData_Item
{
	GENERATED_BODY()
};
