// Copyright Surround, Inc. All Rights Reserved.


#include "SMUserWidget_CharacterState.h"

#include "GameFramework/PlayerState.h"
#include "AbilitySystem/AttributeSets/SMCharacterAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void USMUserWidget_CharacterState::SetASC(UAbilitySystemComponent* InASC)
{
	Super::SetASC(InASC);

	if (!ensure(InASC))
	{
		return;
	}

	InASC->GetGameplayAttributeValueChangeDelegate(USMCharacterAttributeSet::GetPostureGaugeAttribute()).AddUObject(this, &USMUserWidget_CharacterState::OnChangeCurrentHealth);

	InASC->GetGameplayAttributeValueChangeDelegate(USMCharacterAttributeSet::GetMaxPostureGaugeAttribute()).AddUObject(this, &USMUserWidget_CharacterState::OnChangeMaxHealth);

	const USMCharacterAttributeSet* SourceAttributeSet = InASC->GetSet<USMCharacterAttributeSet>();
	if (ensure(SourceAttributeSet))
	{
		CurrentHealth = SourceAttributeSet->GetPostureGauge();
		MaxHealth = SourceAttributeSet->GetMaxPostureGauge();
		UpdateHPBar();
	}
}

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
