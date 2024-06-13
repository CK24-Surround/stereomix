// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMFrontendElementWidget.h"
#include "SMLobbyWidget.h"
#include "SMSettingsWidget.h"
#include "UI/Widget/Common/SMCommonButton.h"
#include "SMMainMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMMainMenuWidget : public USMFrontendElementWidget
{
	GENERATED_BODY()

public:
	USMMainMenuWidget();

protected:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	UFUNCTION()
	void OnLobbyButtonClicked();

	UFUNCTION()
	void OnTutorialButtonClicked();

	UFUNCTION()
	void OnSettingsButtonClicked();

	UFUNCTION()
	void OnQuitButtonClicked();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Frontend Elements", meta=(AllowPrivateAccess))
	TSubclassOf<USMLobbyWidget> LobbyWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Frontend Elements", meta=(AllowPrivateAccess))
	TSubclassOf<USMSettingsWidget> SettingsWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Frontend Elements", meta=(AllowPrivateAccess))
	TSoftObjectPtr<UWorld> TutorialLevel;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta=(AllowPrivateAccess, BindWidget))
	TObjectPtr<USMCommonButton> LobbyButton;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta=(AllowPrivateAccess, BindWidget))
	TObjectPtr<USMCommonButton> TutorialButton;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta=(AllowPrivateAccess, BindWidget))
	TObjectPtr<USMCommonButton> SettingsButton;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta=(AllowPrivateAccess, BindWidget))
	TObjectPtr<USMCommonButton> QuitButton;
};
