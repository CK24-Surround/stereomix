// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/RadialSlider.h"
#include "SMRadialSliderWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMRadialSliderWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void SetValue(float Value) const;

protected:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta = (BindWidget))
	TObjectPtr<URadialSlider> RadialSlider;
};
