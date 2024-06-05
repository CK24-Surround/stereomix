// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UI/ViewModel/Fronted/SMFrontendViewModel.h"
#include "UI/Widget/Frontend/SMFrontendWidget.h"
#include "SMFrontendPlayerController.generated.h"

/**
 * StereoMix Frontend Player Controller
 */
UCLASS()
class STEREOMIX_API ASMFrontendPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASMFrontendPlayerController();

	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<USMFrontendWidget> FrontendWidgetClass;
	
	UPROPERTY(Transient)
	TObjectPtr<USMFrontendWidget> FrontendWidget;
	
	UPROPERTY(Transient)
	TObjectPtr<USMFrontendViewModel> FrontendViewModel;
};
