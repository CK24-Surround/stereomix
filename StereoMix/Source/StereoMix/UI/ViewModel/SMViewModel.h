// Copyright Surround, Inc. All Rights Reserved.

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
	UPROPERTY()
	TWeakObjectPtr<AGameStateBase> OwningGameState;

	UPROPERTY()
	TWeakObjectPtr<APlayerController> OwningPlayerController;
	
	USMViewModel();

	virtual void InitializeViewModel(UWorld* InWorld);

	virtual void BeginDestroy() override;
};
