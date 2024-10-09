#pragma once

#include "SMNoiseBreakData.generated.h"

class UGameplayEffect;
class ASMTile;

USTRUCT()
struct FSMNoiseBreakData
{
	GENERATED_BODY()

	FVector NoiseBreakLocation;

	TArray<TWeakObjectPtr<ASMTile>> TilesToBeTriggered;
};
