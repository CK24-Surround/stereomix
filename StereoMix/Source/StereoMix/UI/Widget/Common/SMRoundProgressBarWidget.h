// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"

#include "SMRoundProgressBarWidget.generated.h"

/**
 *
 */
UCLASS(Abstract)
class STEREOMIX_API USMRoundProgressBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetPercent(float Percent);

private:
	// =============================================================================
	// Bindings

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta = (BindWidget, AllowPrivateAccess))
	TObjectPtr<UImage> ProgressBarImage;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> ProgressBarMaterialInstance;
};
