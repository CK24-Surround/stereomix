// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMViewModel.h"
#include "SMViewModel_MainMenu.generated.h"

/**
 * MainMenu ViewModel
 */
UCLASS(BlueprintType)
class STEREOMIX_API USMViewModel_MainMenu : public USMViewModel
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter=GetUserNameText, meta=(AllowPrivateAccess))
	FText UserNameText;

public:
	const FText& GetUserNameText() const { return UserNameText; }
	void SetUserNameText(const FText& NewUserNameText) { UE_MVVM_SET_PROPERTY_VALUE(UserNameText, NewUserNameText); }

	UFUNCTION(BlueprintCallable, Category="UI")
	void StartGame();

	UFUNCTION(BlueprintCallable, Category="UI")
	void StartTutorial();

	UFUNCTION(BlueprintCallable, Category="UI")
	void ShowSettings();

	UFUNCTION(BlueprintCallable, Category="UI")
	void QuitGame();
};
