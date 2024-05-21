// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "SMViewModel.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonInterat);

/**
 * StereoMix ViewModel base class
 */
UCLASS(Abstract)
class STEREOMIX_API USMViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()
};
