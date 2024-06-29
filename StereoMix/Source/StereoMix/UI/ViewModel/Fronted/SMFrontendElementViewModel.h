// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/ViewModel/SMViewModel.h"

#include "SMFrontendElementViewModel.generated.h"

class USMFrontendViewModel;

/**
 * StereoMix Frontend Element ViewModel
 */
UCLASS(Abstract)
class STEREOMIX_API USMFrontendElementViewModel : public USMViewModel
{
	GENERATED_BODY()

public:
	USMFrontendElementViewModel();
};
