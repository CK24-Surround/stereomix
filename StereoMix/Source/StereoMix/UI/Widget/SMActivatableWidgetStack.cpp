// Copyright Surround, Inc. All Rights Reserved.


#include "SMActivatableWidgetStack.h"

#include "SMActivatableWidget.h"

void USMActivatableWidgetStack::OnWidgetAddedToList(UCommonActivatableWidget& AddedWidget)
{
	Super::OnWidgetAddedToList(AddedWidget);

	if (USMActivatableWidget* Widget = static_cast<USMActivatableWidget*>(&AddedWidget))
	{
		Widget->RegisteredStack = this;
	}
}
