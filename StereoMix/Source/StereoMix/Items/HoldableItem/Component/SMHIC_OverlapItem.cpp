// Copyright Studio Surround. All Rights Reserved.


#include "SMHIC_OverlapItem.h"

#include "Characters/Player/SMPlayerCharacterBase.h"


USMHIC_OverlapItem::USMHIC_OverlapItem()
{
}

bool USMHIC_OverlapItem::CanHolded(AActor* TargetActor) const
{
	ASMPlayerCharacterBase* TargetCharacter = Cast<ASMPlayerCharacterBase>(TargetActor);
	if (!ensureAlways(TargetActor))
	{
		return false;
	}
	
	return true;
}

void USMHIC_OverlapItem::OnHolded(AActor* TargetActor)
{
	ASMPlayerCharacterBase* TargetCharacter = Cast<ASMPlayerCharacterBase>(TargetActor);
	if (!ensureAlways(TargetActor))
	{
		return;
	}

	SetActorHoldingMe(TargetActor);
}

void USMHIC_OverlapItem::BeginPlay()
{
	Super::BeginPlay();
}

