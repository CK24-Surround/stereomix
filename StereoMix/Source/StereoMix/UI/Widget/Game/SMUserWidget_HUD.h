// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMUserWidget.h"
#include "SMUserWidget_GameHUD.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "SMUserWidget_HUD.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMUserWidget_HUD : public USMUserWidget
{
	GENERATED_BODY()

public:
	virtual void SetASC(UAbilitySystemComponent* InASC) override;

protected:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta=(BindWidget))
	TObjectPtr<UCommonActivatableWidgetStack> GameStack;

	UPROPERTY(EditDefaultsOnly, Category = "WidgetClass")
	TSubclassOf<USMUserWidget_GameHUD> GameHUDClass;
};
