// Copyright Surround, Inc. All Rights Reserved.


#include "SMUserWidget_CharacterState.h"

#include "AbilitySystemComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Utilities/SMLog.h"

void USMUserWidget_CharacterState::SetNickname(const FString& InNickname)
{
	TB_Nickname->SetText(FText::FromString(InNickname));
}

void USMUserWidget_CharacterState::OnChangeCurrentHealth(const FOnAttributeChangeData& OnAttributeChangeData)
{
	if (CurrentHealth != OnAttributeChangeData.NewValue)
	{
		CurrentHealth = OnAttributeChangeData.NewValue;
		PlayAnimation(HitAnimation);
		UpdateHPBar();
	}
}

void USMUserWidget_CharacterState::OnChangeMaxHealth(const FOnAttributeChangeData& OnAttributeChangeData)
{
	MaxHealth = OnAttributeChangeData.NewValue;
	UpdateHPBar();
}

void USMUserWidget_CharacterState::UpdateHPBar()
{
	PB_Health->SetPercent(CurrentHealth / MaxHealth);
}
