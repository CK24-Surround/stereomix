// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "SMTutorialHUD.generated.h"

class USMTutorialMission;
class USMTutorialSuccess;
class USMTutorialGuide;
/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMTutorialHUD : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USMTutorialGuide> TutorialGuide;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USMTutorialSuccess> TutorialSuccess;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USMTutorialMission> TutorialMission;
};
