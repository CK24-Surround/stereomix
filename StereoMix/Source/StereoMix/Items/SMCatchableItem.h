// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMItem.h"
#include "Interfaces/SMCatchInteractionInterface.h"
#include "SMCatchableItem.generated.h"

class USMCatchInteractionComponent_CatchableItem_AttributeChanger;

UCLASS(Abstract)
class STEREOMIX_API ASMCatchableItem : public ASMItem, public ISMCatchInteractionInterface
{
	GENERATED_BODY()

public:
	ASMCatchableItem();

public:
	virtual USMCatchInteractionComponent* GetCatchInteractionComponent() override PURE_VIRTUAL(ASMCatchableItem::GetCatchInteractionComponent, return nullptr;)

protected:
	UPROPERTY(VisibleAnywhere, Category = "Root")
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
};
