// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "SMViewModel.generated.h"

/**
 * StereoMix ViewModel base class
 */
UCLASS(Abstract)
class STEREOMIX_API USMViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	USMViewModel();
};
