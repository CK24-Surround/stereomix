// Copyright Surround, Inc. All Rights Reserved.


#include "SMFrontendWidgetStack.h"

USMFrontendWidgetStack::USMFrontendWidgetStack()
{
	TransitionType = ECommonSwitcherTransition::FadeOnly;
	TransitionDuration = 0.0f;
	TransitionFallbackStrategy = ECommonSwitcherTransitionFallbackStrategy::Previous;
}
