// Fill out your copyright notice in the Description page of Project Settings.


#include "SMCharacterStateWidget.h"

#include "Components/TextBlock.h"

void USMCharacterStateWidget::UpdateNickname(const FString& InNickname)
{
	TB_Health->SetText(FText::FromString(InNickname));
}

void USMCharacterStateWidget::UpdateHealth(float CurrentHealth, float MaxHealth) {}
