// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/Frontend/SMFrontendElementWidget.h"
#include "SMCustomServerWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMCustomServerWidget : public USMFrontendElementWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void ConnectToServer(FString Url, FString UserName);
};
