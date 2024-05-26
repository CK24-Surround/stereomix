// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMViewModel.h"
#include "StereoMix.h"
#include "SMViewModel_Lobby.generated.h"

/**
 * Lobby ViewModel
 */
UCLASS()
class STEREOMIX_API USMViewModel_Lobby : public USMViewModel
{
	GENERATED_BODY()

public:
	// ==============================================================
	// View

	UFUNCTION(BlueprintCallable, Category="UI")
	void CreateRoom();

	UFUNCTION(BlueprintCallable, Category="UI")
	void QuickMatch();

	UFUNCTION(BlueprintCallable, Category="UI")
	void JoinRoom();

	UFUNCTION(BlueprintCallable, Category="UI")
	void Back();
};
