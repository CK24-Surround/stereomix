// Copyright Surround, Inc. All Rights Reserved.


#include "SMUserWidget.h"

#include "AbilitySystemComponent.h"

void USMUserWidget::SetASC(UAbilitySystemComponent* InASC)
{
	if (ensure(InASC))
	{
		ASC = InASC;
	}
}

UAbilitySystemComponent* USMUserWidget::GetAbilitySystemComponent() const
{
	return ASC.Get();
}
