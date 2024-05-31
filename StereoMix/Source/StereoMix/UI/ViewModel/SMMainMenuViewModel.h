// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMViewModel.h"
#include "SMMainMenuViewModel.generated.h"

/**
 * MainMenu ViewModel
 */
UCLASS(BlueprintType)
class STEREOMIX_API USMMainMenuViewModel : public USMViewModel
{
	GENERATED_BODY()

public:
	virtual void InitializeViewModel(UWorld* InWorld) override;
	
	UFUNCTION(BlueprintCallable)
	void StartGame();

	UFUNCTION(BlueprintCallable)
	void StartTutorial();

	UFUNCTION(BlueprintCallable)
	void QuitGame();

private:
	UPROPERTY(BlueprintReadWrite, FieldNotify, meta=(AllowPrivateAccess))
	FText UserName;
};
