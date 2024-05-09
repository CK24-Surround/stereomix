// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMCatchableItem.h"
#include "SMCatchableItem_Heal.generated.h"

UCLASS()
class STEREOMIX_API ASMCatchableItem_Heal : public ASMCatchableItem
{
	GENERATED_BODY()

public:
	ASMCatchableItem_Heal();

	virtual USMCatchInteractionComponent* GetCatchInteractionComponent() override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "CIC")
	TObjectPtr<USMCatchInteractionComponent_CatchableItem_Heal> CIC; 
};
