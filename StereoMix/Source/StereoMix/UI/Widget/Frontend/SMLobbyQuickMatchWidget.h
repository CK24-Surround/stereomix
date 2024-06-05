// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMLobbyProcessWidget.h"
#include "SMLobbyQuickMatchWidget.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMLobbyQuickMatchWidget : public USMLobbyProcessWidget
{
	GENERATED_BODY()

public:
	USMLobbyQuickMatchWidget();

protected:
	virtual void NativeOnActivated() override;

	virtual void NativeOnDeactivated() override;

	UFUNCTION()
	virtual void OnQuickMatchResponse(EQuickMatchResult Result, const FString& ServerUrl);
};
