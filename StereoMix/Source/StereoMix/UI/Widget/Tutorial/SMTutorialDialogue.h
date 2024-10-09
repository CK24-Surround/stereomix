// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "SMTutorialDialogue.generated.h"

class UCommonTextBlock;
/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMTutorialDialogue : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	USMTutorialDialogue();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> NameTextBlock;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> ScriptTextBlock;
};
