// Fill out your copyright notice in the Description page of Project Settings.

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

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USMUserWidget_Scoreboard> Scoreboard;
};
