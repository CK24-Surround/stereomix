// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "UI/Widget/SMUserWidget.h"
#include "SMUserWidget_StaminaSkillGaugeDummyBar.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS()
class STEREOMIX_API USMUserWidget_StaminaSkillGaugeDummyBar : public USMUserWidget
{
	GENERATED_BODY()

public:
	void OnStaminaChanged(const FOnAttributeChangeData& OnAttributeChangeData);

	void UpdateStaminaBar();

	void OnSkillGaugeChanged(const FOnAttributeChangeData& OnAttributeChangeData);

	void UpdateSkillGaugeBar();

	float Stamina = 0.0f;

	float MaxStamina = 0.0f;

	float SkillGauge = 0.0f;

	float MaxSkillGauge = 0.0f;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> StaminaBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> SkillGaugeBar;
};
