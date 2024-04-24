// Fill out your copyright notice in the Description page of Project Settings.


#include "SMUserWidget_PhaseTimerBar.h"

#include "Components/Image.h"

bool USMUserWidget_PhaseTimerBar::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	PhaseTimerBarMaterialInstanceDynamic = PhaseTimerBar->GetDynamicMaterial();
	if (!ensureAlways(PhaseTimerBarMaterialInstanceDynamic.Get()))
	{
		return false;
	}

	return true;
}

void USMUserWidget_PhaseTimerBar::SetRatio(float InRatio)
{
	PhaseTimerBarMaterialInstanceDynamic->SetScalarParameterValue(TEXT("Ratio"), InRatio);
}
