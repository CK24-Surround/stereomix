// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/HoldInteraction/SMHoldInteractionComponent.h"
#include "SMHIC_TrainingDummy.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMHIC_TrainingDummy : public USMHoldInteractionComponent
{
	GENERATED_BODY()

public:
	USMHIC_TrainingDummy();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
