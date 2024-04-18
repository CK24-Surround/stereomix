// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SMUserWidget.h"
#include "SMUserWidget_Scoreboard.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class STEREOMIX_API USMUserWidget_Scoreboard : public USMUserWidget
{
	GENERATED_BODY()

public:
	virtual void SetASC(UAbilitySystemComponent* InASC) override;

protected:
	void OnChangeRoundTime(int32 RoundTime);

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Timer;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> EDMScore;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> FutureBaseScore;
};
