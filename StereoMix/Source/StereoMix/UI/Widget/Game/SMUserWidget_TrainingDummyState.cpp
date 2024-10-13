// Copyright Studio Surround. All Rights Reserved.


#include "SMUserWidget_TrainingDummyState.h"

#include "Components/ProgressBar.h"

void USMUserWidget_TrainingDummyState::SetHP(float CurrentHP, float MaxHP)
{
	if (HPBar)
	{
		const float Ratio = MaxHP != 0.0f ? CurrentHP / MaxHP : 0.0f;
		HPBar->SetPercent(Ratio);
	}
}
