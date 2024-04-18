// Fill out your copyright notice in the Description page of Project Settings.


#include "SMCharacterStateWidget.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AttributeSets/SMCharacterAttributeSet.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void USMCharacterStateWidget::SetASC(UAbilitySystemComponent* InASC)
{
	Super::SetASC(InASC);

	APlayerState* PlayerState = Cast<APlayerState>(InASC->GetOwnerActor());
	if (ensure(PlayerState))
	{
		UpdateNickname(PlayerState->GetPlayerName());
	}

	InASC->GetGameplayAttributeValueChangeDelegate(USMCharacterAttributeSet::GetPostureGaugeAttribute()).AddUObject(this, &USMCharacterStateWidget::OnChangeCurrentHealth);
	InASC->GetGameplayAttributeValueChangeDelegate(USMCharacterAttributeSet::GetMaxPostureGaugeAttribute()).AddUObject(this, &USMCharacterStateWidget::OnChangeMaxHealth);

	const USMCharacterAttributeSet* SourceAttributeSet = InASC->GetSet<USMCharacterAttributeSet>();
	if (ensure(SourceAttributeSet))
	{
		CurrentHealth = SourceAttributeSet->GetPostureGauge();
		MaxHealth = SourceAttributeSet->GetMaxPostureGauge();
		UpdateHealth();
	}
}

void USMCharacterStateWidget::UpdateNickname(const FString& InNickname)
{
	TB_Nickname->SetText(FText::FromString(InNickname));
}

void USMCharacterStateWidget::OnChangeCurrentHealth(const FOnAttributeChangeData& OnAttributeChangeData)
{
	CurrentHealth = OnAttributeChangeData.NewValue;
	UpdateHealth();
}

void USMCharacterStateWidget::OnChangeMaxHealth(const FOnAttributeChangeData& OnAttributeChangeData)
{
	MaxHealth = OnAttributeChangeData.NewValue;
	UpdateHealth();
}

void USMCharacterStateWidget::UpdateHealth()
{
	PB_Health->SetPercent(CurrentHealth / MaxHealth);
}
