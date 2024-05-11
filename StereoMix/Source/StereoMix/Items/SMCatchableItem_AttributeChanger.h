// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMCatchableItem.h"
#include "SMCatchableItem_AttributeChanger.generated.h"

class ASMTile;

UCLASS()
class STEREOMIX_API ASMCatchableItem_AttributeChanger : public ASMCatchableItem
{
	GENERATED_BODY()

public:
	ASMCatchableItem_AttributeChanger();

public:
	virtual USMCatchInteractionComponent* GetCatchInteractionComponent() override;

public:
	virtual void ActivateItem() override;

protected:
	/** 버프가 적용될때마다 호출되야합니다. */
	void HandleApplyItem();

protected:
	UPROPERTY(VisibleAnywhere, Category = "CIC")
	TObjectPtr<USMCatchInteractionComponent_CatchableItem_AttributeChanger> CIC;

public:
	TArray<TWeakObjectPtr<ASMTile>> TriggeredTiles;

protected:
	TArray<TWeakObjectPtr<AActor>> ApplyActors;
};
