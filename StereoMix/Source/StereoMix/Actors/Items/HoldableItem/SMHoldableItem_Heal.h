// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMHoldableItemBase.h"
#include "SMHoldableItem_Heal.generated.h"

UCLASS()
class STEREOMIX_API ASMHoldableItem_Heal : public ASMHoldableItemBase
{
	GENERATED_BODY()

public:
	ASMHoldableItem_Heal(const FObjectInitializer& ObjectInitializer);

	virtual void ActivateItemByNoiseBreak(const UWorld* World, const TArray<ASMTile*>& TilesToBeCaptured, AActor* InActivator, const TOptional<ESMTeam>& TeamOption) override;

	void OnHeldStateEntry() const;

	void OnHeldStateExit() const;
};
