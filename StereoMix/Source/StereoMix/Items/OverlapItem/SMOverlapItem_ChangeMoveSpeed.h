// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMOverlapItemBase.h"
#include "SMOverlapItem_ChangeMoveSpeed.generated.h"

UCLASS(Abstract)
class STEREOMIX_API ASMOverlapItem_ChangeMoveSpeed : public ASMOverlapItemBase
{
	GENERATED_BODY()

protected:
	virtual void ActivateItem(AActor* InActivator) override;

	UPROPERTY(EditAnywhere, Category = "Design")
	float MoveSpeedMultiplier = 1.5f;
	
	UPROPERTY(EditAnywhere, Category = "Design")
	float Duration = 5.0f;
};
