// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/SMUserWidget.h"

#include "SMUserWidget_PhaseTimerBar.generated.h"

class UImage;
/**
 *
 */
UCLASS(Abstract)
class STEREOMIX_API USMUserWidget_PhaseTimerBar : public USMUserWidget
{
	GENERATED_BODY()

protected:
	virtual bool Initialize() override;

public:
	void SetRatio(float InRatio);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PhaseTimerBar;

	UPROPERTY(Transient)
	TWeakObjectPtr<UMaterialInstanceDynamic> PhaseTimerBarMaterialInstanceDynamic;
};
