// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "SMTutorialHUD.generated.h"

class USMTutorialDialogue;
/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMTutorialHUD : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	USMTutorialDialogue* GetTutorialDialogue() const { return TutorialDialogue; }

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USMTutorialDialogue> TutorialDialogue;
};
