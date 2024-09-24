// Copyright Studio Surround. All Rights Reserved.


#include "SMHIC_OverlapItem.h"

#include "Characters/Player/SMPlayerCharacterBase.h"


bool USMHIC_OverlapItem::CanHolded(AActor* TargetActor) const
{
	if (!ensureAlways(TargetActor))
	{
		return false;
	}

	return true;
}

void USMHIC_OverlapItem::OnHolded(AActor* TargetActor)
{
	if (!ensureAlways(TargetActor))
	{
		return;
	}

	SetActorHoldingMe(TargetActor);
}
