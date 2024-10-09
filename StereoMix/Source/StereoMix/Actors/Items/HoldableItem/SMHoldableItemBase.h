// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Items/SMItemBase.h"
#include "Actors/Tiles/SMTile.h"
#include "Data/SMTeam.h"
#include "Interfaces/SMHoldInteractionInterface.h"
#include "SMHoldableItemBase.generated.h"

UCLASS(Abstract)
class STEREOMIX_API ASMHoldableItemBase : public ASMItemBase, public ISMHoldInteractionInterface
{
	GENERATED_BODY()

public:
	ASMHoldableItemBase(const FObjectInitializer& ObjectInitializer);

	virtual USMHoldInteractionComponent* GetHoldInteractionComponent() const override { return HIC; }

	virtual void ActivateItemByNoiseBreak(AActor* InActivator, const TArray<TWeakObjectPtr<ASMTile>>& TilesToBeCaptured) {}

	const static FName HICName;

protected:
	UPROPERTY(VisibleAnywhere, Category = "HoldInteraction")
	TObjectPtr<USMHoldInteractionComponent> HIC;
};
