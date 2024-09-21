// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/SMHoldInteractionInterface.h"
#include "Items/SMItem.h"
#include "SMHoldableItemBase.generated.h"

UCLASS(Abstract)
class STEREOMIX_API ASMHoldableItemBase : public ASMItem, public ISMHoldInteractionInterface
{
	GENERATED_BODY()

public:
	ASMHoldableItemBase(const FObjectInitializer& ObjectInitializer);

	virtual USMHoldInteractionComponent* GetHoldInteractionComponent() const override { return HIC; }

	const static FName HICName;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Root")
	TObjectPtr<USceneComponent> SceneComponent;
	
	UPROPERTY(VisibleAnywhere, Category = "HoldInteraction")
	TObjectPtr<USMHoldInteractionComponent> HIC;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
};
