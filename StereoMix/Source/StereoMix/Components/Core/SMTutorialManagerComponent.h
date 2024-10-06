// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SMTutorialManagerComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMTutorialManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USMTutorialManagerComponent();

	virtual void BeginPlay() override;
};
