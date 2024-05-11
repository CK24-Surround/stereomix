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

public:
	/** 아이템을 활성화합니다. 서브클래스에서 직접 구현해줘야합니다. */
	virtual void ActivateItem() PURE_VIRTUAL(ASMCatchableItem::ActivateItem)

protected:
	UPROPERTY(VisibleAnywhere, Category = "Root")
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
};
