﻿// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"

#include "SMViewModel.generated.h"

/**
 * StereoMix ViewModel base class
 */
UCLASS(Abstract, BlueprintType)
class STEREOMIX_API USMViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	USMViewModel();

	virtual void InitViewModel(APlayerController* PlayerController);

	APlayerController* GetOwningController() const { return OwningController.Get(); }

private:
	TWeakObjectPtr<APlayerController> OwningController;
};
