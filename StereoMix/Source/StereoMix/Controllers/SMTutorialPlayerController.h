// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UI/Widget/Tutorial/SMTutorialWidget.h"
#include "SMTutorialPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API ASMTutorialPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASMTutorialPlayerController();

	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<USMTutorialWidget> TutorialWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<USMTutorialWidget> TutorialWidget;
};
