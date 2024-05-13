// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SMItem.h"
#include "Data/SMTeam.h"
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
	/** 게임플레이 큐를 재생할때 사용할 태그입니다.*/
	UPROPERTY(EditAnywhere, Category = "Item", meta = (Categories = "GameplayCue"))
	FGameplayTag SmashGCTag;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Root")
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
};
