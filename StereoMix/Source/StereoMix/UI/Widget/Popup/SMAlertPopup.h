// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonRichTextBlock.h"
#include "SMPopup.h"

#include "SMAlertPopup.generated.h"

/**
 * StereoMix Alert Popup Widget
 */
UCLASS(Abstract)
class STEREOMIX_API USMAlertPopup : public USMPopup
{
	GENERATED_BODY()

public:
	USMAlertPopup();

	void SetAlertText(const FText& InAlertText);

protected:
	virtual void PerformSubmit() override;

private:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UCommonRichTextBlock> AlertTextBlock;
};
