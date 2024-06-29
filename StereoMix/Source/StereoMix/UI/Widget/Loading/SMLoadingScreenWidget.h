// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonTextBlock.h"
#include "CommonUserWidget.h"

#include "SMLoadingScreenWidget.generated.h"

/**
 *
 */
UCLASS(Abstract)
class STEREOMIX_API USMLoadingScreenWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	void ShowLoadingScreen();

	void HideLoadingScreen();

	void SetLoadingText(const FText& Text) const;

private:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Animation", meta = (BindWidgetAnim, AllowPrivateAccess))
	TObjectPtr<UWidgetAnimation> TransitionAnim;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta = (BindWidget, AllowPrivateAccess))
	TObjectPtr<UCommonTextBlock> LoadingTextBlock;
};
