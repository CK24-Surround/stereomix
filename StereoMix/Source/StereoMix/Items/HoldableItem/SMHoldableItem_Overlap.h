// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMHoldableItemBase.h"
#include "SMHoldableItem_Overlap.generated.h"

UCLASS(Abstract)
class STEREOMIX_API ASMHoldableItem_Overlap : public ASMHoldableItemBase
{
	GENERATED_BODY()

public:
	ASMHoldableItem_Overlap(const FObjectInitializer& ObjectInitializer);
};
