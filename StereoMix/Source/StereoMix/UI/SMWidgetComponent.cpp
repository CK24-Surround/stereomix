// Copyright Surround, Inc. All Rights Reserved.


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
