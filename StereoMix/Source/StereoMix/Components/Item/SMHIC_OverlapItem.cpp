// Copyright Studio Surround. All Rights Reserved.


#include "SMHIC_OverlapItem.h"



bool USMHIC_OverlapItem::CanBeHeld(AActor* Instigator) const
{
	if (!ensureAlways(Instigator))
	{
		return false;
	}

	return true;
}

void USMHIC_OverlapItem::OnHeld(AActor* Instigator)
{
	if (!ensureAlways(Instigator))
	{
		return;
	}

	SetActorHoldingMe(Instigator);
}
