// Copyright Studio Surround. All Rights Reserved.


#include "SMHoldableItem_Overlap.h"

#include "Component/SMHIC_OverlapItem.h"

ASMHoldableItem_Overlap::ASMHoldableItem_Overlap(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USMHIC_OverlapItem>(ASMHoldableItemBase::HICName)) {}
