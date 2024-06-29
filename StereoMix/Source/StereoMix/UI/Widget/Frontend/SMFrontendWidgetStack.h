// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/SMActivatableWidgetStack.h"

#include "SMFrontendWidgetStack.generated.h"

class USMFrontendElementWidget;
/**
 * StereoMix frontend widget stack
 */
UCLASS()
class STEREOMIX_API USMFrontendWidgetStack : public USMActivatableWidgetStack
{
	GENERATED_BODY()

public:
	USMFrontendWidgetStack();
};
