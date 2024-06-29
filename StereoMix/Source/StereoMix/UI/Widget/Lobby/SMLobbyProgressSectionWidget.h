// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonTextBlock.h"
#include "CommonUserWidget.h"

#include "SMLobbyProgressSectionWidget.generated.h"

/**
 *
 */
UCLASS(Abstract)
class STEREOMIX_API USMLobbyProgressSectionWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	void SetProgressText(const FText& InText);

private:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UCommonTextBlock> ProgressTextBlock;
};
