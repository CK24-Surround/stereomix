// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/Common/SMRadialSliderWidget.h"
#include "SMItemSpawnTimerWidget.generated.h"

class ASMItemSpawner;
/**
 * 
 */
UCLASS()
class STEREOMIX_API USMItemSpawnTimerWidget : public USMRadialSliderWidget
{
	GENERATED_BODY()

public:
	void SetItemSpawner(ASMItemSpawner* ItemSpawner);

	UFUNCTION()
	void SetCooldownPercent(float RemainTime, float Percent);
};
