// Copyright Surround, Inc. All Rights Reserved.


#include "SMRoundProgressBarWidget.h"

void USMRoundProgressBarWidget::SetPercent(const float Percent)
{
	if (!ProgressBarMaterialInstance)
	{
		ProgressBarMaterialInstance = ProgressBarImage->GetDynamicMaterial();
		ProgressBarImage->SetBrushFromMaterial(ProgressBarMaterialInstance);
	}

	ProgressBarMaterialInstance->SetScalarParameterValue("Percent", Percent);
}
