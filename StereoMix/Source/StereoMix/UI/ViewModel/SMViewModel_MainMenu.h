// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMViewModel.h"
#include "SMViewModel_MainMenu.generated.h"

/**
 * MainMenu ViewModel
 */
UCLASS()
class STEREOMIX_API USMViewModel_MainMenu : public USMViewModel
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, meta=(AllowPrivateAccess))
	FText UserName;

public:
	const FText& GetUserName() const { return UserName; }
	void SetUserName(const FText& NewUserName) { UE_MVVM_SET_PROPERTY_VALUE(UserName, NewUserName); }
	
	UFUNCTION(BlueprintCallable, Category="UI")
	void StartGame();

	UFUNCTION(BlueprintCallable, Category="UI")
	void StartTutorial();

	UFUNCTION(BlueprintCallable, Category="UI")
	void ShowSettings();

	UFUNCTION(BlueprintCallable, Category="UI")
	void QuitGame() const;
};
