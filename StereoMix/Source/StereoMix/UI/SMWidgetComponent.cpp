// Fill out your copyright notice in the Description page of Project Settings.


#include "SMWidgetComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "SMUserWidget.h"

void USMWidgetComponent::SetWidget(UUserWidget* InWidget)
{
	Super::SetWidget(InWidget);

	USMUserWidget* SMUserWidget = Cast<USMUserWidget>(GetWidget());
	if (SMUserWidget)
	{
		UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
		if (ensure(SourceASC))
		{
			SMUserWidget->SetASC(SourceASC);
		}
	}
}
