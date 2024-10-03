// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMHIC_ItemBase.h"
#include "SMHIC_OverlapItem.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMHIC_OverlapItem : public USMHIC_ItemBase
{
	GENERATED_BODY()

public:
	virtual bool CanHolded(AActor* Instigator) const override;

	virtual void OnHolded(AActor* Instigator) override;
};
