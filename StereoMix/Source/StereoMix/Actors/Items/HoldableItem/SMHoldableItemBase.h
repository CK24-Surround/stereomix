// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Items/SMItemBase.h"
#include "Interfaces/SMHoldInteractionInterface.h"
#include "SMHoldableItemBase.generated.h"

UCLASS(Abstract)
class STEREOMIX_API ASMHoldableItemBase : public ASMItemBase, public ISMHoldInteractionInterface
{
	GENERATED_BODY()

public:
	ASMHoldableItemBase(const FObjectInitializer& ObjectInitializer);

	virtual USMHoldInteractionComponent* GetHoldInteractionComponent() const override { return HIC; }

	const static FName HICName;

protected:
	UPROPERTY(VisibleAnywhere, Category = "HoldInteraction")
	TObjectPtr<USMHoldInteractionComponent> HIC;
};
