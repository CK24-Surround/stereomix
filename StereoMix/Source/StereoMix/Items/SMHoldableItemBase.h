// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HoldInteraction/SMHoldInteractionComponent.h"
#include "SMHoldableItemBase.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMHoldableItemBase : public USMHoldInteractionComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USMHoldableItemBase();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
