// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "SMTutorialMission.generated.h"

class UCommonTextBlock;
/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMTutorialMission : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> TargetTextBlock;

	UPROPERTY(Transient, meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> DetailTextBlock;

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> ShowMissionAnimation;

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> HideMissionAnimation;
};
