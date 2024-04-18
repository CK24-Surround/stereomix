// Fill out your copyright notice in the Description page of Project Settings.


#include "SMWidgetComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "SMUserWidget.h"

void USMWidgetComponent::InitWidget()
{
	Super::InitWidget();

	USMUserWidget* SMUserWidget = Cast<USMUserWidget>(GetWidget());
	if (ensure(SMUserWidget))
	{
		UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
		if (ensure(SourceASC))
		{
			SMUserWidget->SetASC(SourceASC);
		}
	}
}
