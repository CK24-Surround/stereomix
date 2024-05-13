// Copyright Surround, Inc. All Rights Reserved.


#include "SMItem.h"

ASMItem::ASMItem()
{
	bReplicates = true;
	bAlwaysRelevant = true;
}

void ASMItem::ActivateItem(AActor* InActivator)
{
	(void)OnUsedItem.ExecuteIfBound();
}
