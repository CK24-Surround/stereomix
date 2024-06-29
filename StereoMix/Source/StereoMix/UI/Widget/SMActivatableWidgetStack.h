// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

#include "SMActivatableWidgetStack.generated.h"

class USMActivatableWidget;

/**
 *
 */
UCLASS(ClassGroup = "StereoMix Common UI")
class STEREOMIX_API USMActivatableWidgetStack : public UCommonActivatableWidgetStack
{
	GENERATED_BODY()

public:
	template<typename T = USMActivatableWidget>
	T* RegisterWidget(TSubclassOf<T> WidgetClass)
	{
		return GeneratedWidgetsPool.GetOrCreateInstance(WidgetClass);
	}
};
