// Copyright Surround, Inc. All Rights Reserved.


#include "SMGamePanel.h"

#include "AbilitySystemComponent.h"

void USMGamePanel::SetASC(UAbilitySystemComponent* InASC)
{
	if (ensure(InASC))
	{
		ASC = InASC;
	}
}

UAbilitySystemComponent* USMGamePanel::GetAbilitySystemComponent() const
{
	return ASC.Get();
}
