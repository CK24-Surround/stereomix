// Copyright Surround, Inc. All Rights Reserved.


#include "SMCatchableItem_Heal.h"

#include "Components/SMCatchInteractionComponent_CatchableItem_Heal.h"


ASMCatchableItem_Heal::ASMCatchableItem_Heal()
{
	CIC = CreateDefaultSubobject<USMCatchInteractionComponent_CatchableItem_Heal>(TEXT("CIC"));
}

USMCatchInteractionComponent* ASMCatchableItem_Heal::GetCatchInteractionComponent()
{
	return CIC;
}
