// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMOverlapItemBase.h"
#include "SMOverlapItem_ChangerMoveSpeed.generated.h"

UCLASS(Abstract)
class STEREOMIX_API ASMOverlapItem_ChangerMoveSpeed : public ASMOverlapItemBase
{
	GENERATED_BODY()

protected:
	virtual void ActivateItem(AActor* InActivator) override;
};
