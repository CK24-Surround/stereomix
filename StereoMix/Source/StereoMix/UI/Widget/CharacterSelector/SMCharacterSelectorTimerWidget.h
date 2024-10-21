// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "SMCharacterSelectorTimerWidget.generated.h"

class UCommonTextBlock;
/**
 * 
 */
UCLASS()
class STEREOMIX_API USMCharacterSelectorTimerWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	void SetRemainTime(int32 Time) const;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> RemainTime;
};
