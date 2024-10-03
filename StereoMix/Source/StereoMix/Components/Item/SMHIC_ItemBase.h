// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/HoldInteraction/SMHoldInteractionComponent.h"
#include "SMHIC_ItemBase.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMHIC_ItemBase : public USMHoldInteractionComponent
{
	GENERATED_BODY()

public:
	USMHIC_ItemBase();

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
