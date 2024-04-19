// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SMUserWidget.h"
#include "SMUserWidget_VictoryDefeat.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class STEREOMIX_API USMUserWidget_VictoryDefeat : public USMUserWidget
{
	GENERATED_BODY()

public:
	virtual void SetASC(UAbilitySystemComponent* InASC) override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Victory;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Defeat;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Draw;
};
