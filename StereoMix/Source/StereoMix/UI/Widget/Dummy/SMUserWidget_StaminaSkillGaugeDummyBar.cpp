// Copyright Studio Surround. All Rights Reserved.


#include "SMUserWidget_StaminaSkillGaugeDummyBar.h"

#include "Components/ProgressBar.h"


void USMUserWidget_StaminaSkillGaugeDummyBar::OnStaminaChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	Stamina = OnAttributeChangeData.NewValue;
	UpdateStaminaBar();
}

void USMUserWidget_StaminaSkillGaugeDummyBar::OnSkillGaugeChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	SkillGauge = OnAttributeChangeData.NewValue;
	UpdateSkillGaugeBar();
}

void USMUserWidget_StaminaSkillGaugeDummyBar::UpdateStaminaBar()
{
	Stamina = FMath::Clamp(Stamina, 0.0, MaxStamina);
	StaminaBar->SetPercent(Stamina / MaxStamina);
}

void USMUserWidget_StaminaSkillGaugeDummyBar::UpdateSkillGaugeBar()
{
	SkillGauge = FMath::Clamp(SkillGauge, 0.0, MaxSkillGauge);
	SkillGaugeBar->SetPercent(SkillGauge / MaxSkillGauge);
}
