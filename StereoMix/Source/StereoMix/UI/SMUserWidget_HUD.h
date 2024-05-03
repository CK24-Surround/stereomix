// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMUserWidget.h"
#include "SMUserWidget_HUD.generated.h"

class USMUserWidget_Scoreboard;

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
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USMUserWidget_Scoreboard> Scoreboard;
};
