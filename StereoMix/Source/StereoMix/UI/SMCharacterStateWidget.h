// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SMCharacterStateWidget.generated.h"

class UProgressBar;
class UTextBlock;
/**
 * 
 */
UCLASS()
class STEREOMIX_API USMCharacterStateWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateNickname(const FString& InNickname);

	void UpdateHealth(float CurrentHealth, float MaxHealth);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TB_Health;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> PB_Health;
};
