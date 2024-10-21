// Copyright Studio Surround. All Rights Reserved.


#include "SMCharacterSelectorTimerWidget.h"

#include "CommonTextBlock.h"

void USMCharacterSelectorTimerWidget::SetRemainTime(int32 Time) const
{
	RemainTime->SetText(FText::AsNumber(Time));
}
