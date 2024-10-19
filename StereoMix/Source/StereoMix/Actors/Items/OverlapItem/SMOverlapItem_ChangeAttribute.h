// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SMOverlapItemBase.h"
#include "SMOverlapItem_ChangeAttribute.generated.h"

UCLASS(Abstract)
class STEREOMIX_API ASMOverlapItem_ChangeAttribute : public ASMOverlapItemBase
{
	GENERATED_BODY()

protected:
	virtual void ActivateItem(AActor* InActivator) override;

	UPROPERTY(EditAnywhere, Category = "Design")
	FGameplayTag DataTag;

	UPROPERTY(EditAnywhere, Category = "Design")
	float Magnitude = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Design")
	FGameplayTag GCTag;
};
