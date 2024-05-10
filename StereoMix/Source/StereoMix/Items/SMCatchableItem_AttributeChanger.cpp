// Copyright Surround, Inc. All Rights Reserved.


#include "SMCatchableItem_AttributeChanger.h"

#include "Components/SMCatchInteractionComponent_CatchableItem_AttributeChanger.h"


ASMCatchableItem_AttributeChanger::ASMCatchableItem_AttributeChanger()
{
	CIC = CreateDefaultSubobject<USMCatchInteractionComponent_CatchableItem_AttributeChanger>(TEXT("CIC"));
}

USMCatchInteractionComponent* ASMCatchableItem_AttributeChanger::GetCatchInteractionComponent()
{
	return CIC;
}
