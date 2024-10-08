// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMHoldableItemBase.h"
#include "SMHoldableItem_ChangeAttribute.generated.h"

UCLASS()
class STEREOMIX_API ASMHoldableItem_ChangeAttribute : public ASMHoldableItemBase
{
	GENERATED_BODY()

public:
	ASMHoldableItem_ChangeAttribute(const FObjectInitializer& ObjectInitializer);

	virtual void ActivateItemByNoiseBreak(const UWorld* World, const TArray<ASMTile*>& TilesToBeCaptured, AActor* InActivator, const TOptional<ESMTeam>& TeamOption) override;

	virtual void OnHeldStateEntry() override;

	virtual void OnHeldStateExit() override;
};
