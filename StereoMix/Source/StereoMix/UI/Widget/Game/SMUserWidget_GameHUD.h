// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMGamePanel.h"

#include "SMUserWidget_GameHUD.generated.h"

class USMUserWidget_Scoreboard;

/**
 *
 */
UCLASS(Abstract)
class STEREOMIX_API USMUserWidget_GameHUD : public USMGamePanel
{
	GENERATED_BODY()

public:
	virtual void SetASC(UAbilitySystemComponent* InASC) override;

	virtual void NativeOnActivated() override;

	virtual UWidget* NativeGetDesiredFocusTarget() const override;

	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

protected:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta = (BindWidget))
	TObjectPtr<USMUserWidget_Scoreboard> ScoreboardWidget;
};
