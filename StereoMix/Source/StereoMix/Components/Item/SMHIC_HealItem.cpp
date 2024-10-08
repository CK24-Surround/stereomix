// Copyright Studio Surround. All Rights Reserved.


#include "SMHIC_HealItem.h"

#include "Actors/Items/HoldableItem/SMHoldableItem_Heal.h"


bool USMHIC_HealItem::CanBeHeld(AActor* Instigator) const
{
	if (!ensureAlways(Instigator))
	{
		return false;
	}

	return true;
}

void USMHIC_HealItem::OnHeld(AActor* Instigator)
{
	if (!ensureAlways(Instigator))
	{
		return;
	}

	SetActorHoldingMe(Instigator);
}
