// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMCatchableItem.h"
#include "SMCatchableItem_AttributeChanger.generated.h"

UCLASS()
class STEREOMIX_API ASMCatchableItem_AttributeChanger : public ASMCatchableItem
{
	GENERATED_BODY()

public:
	ASMCatchableItem_AttributeChanger();

	virtual USMCatchInteractionComponent* GetCatchInteractionComponent() override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "CIC")
	TObjectPtr<USMCatchInteractionComponent_CatchableItem_AttributeChanger> CIC; 
};
