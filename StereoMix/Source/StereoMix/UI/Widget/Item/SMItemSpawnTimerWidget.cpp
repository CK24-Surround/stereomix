// Copyright Surround, Inc. All Rights Reserved.


#include "SMItemSpawnTimerWidget.h"

#include "Actors/Items/SMItemSpawner.h"


void USMItemSpawnTimerWidget::SetItemSpawner(ASMItemSpawner* ItemSpawner)
{
	ItemSpawner->OnRemainingCooldownChanged.AddDynamic(this, &ThisClass::SetCooldownPercent);
}

void USMItemSpawnTimerWidget::SetCooldownPercent(float RemainTime, float Percent)
{
	SetValue(Percent);
	SetVisibility(Percent >= 1.0f ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
}

