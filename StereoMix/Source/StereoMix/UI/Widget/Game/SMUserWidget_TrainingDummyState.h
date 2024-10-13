// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/SMUserWidget.h"
#include "SMUserWidget_TrainingDummyState.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMUserWidget_TrainingDummyState : public USMUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void SetHP(float CurrentHP, float MaxHP);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HPBar;
};
