// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "SMWidgetComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEREOMIX_API USMWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	virtual void SetWidget(UUserWidget* Widget) override;
};
