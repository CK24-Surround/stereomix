// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/Frontend/SMFrontendElementWidget.h"

#include "SMLobbyWidget.generated.h"

class USMPopup;
enum class EGrpcServiceState : uint8;
class USMClientLobbySubsystem;
class USMLobbyRoomCodePopup;
class USMLobbyJoinRoomWidget;
class USMLobbyQuickMatchWidget;
class USMLobbyCreateRoomWidget;
class UCommonActivatableWidgetStack;
class USMCreateRoomPopup;

/**
 * StereoMix Lobby Widget
 */
UCLASS(Abstract)
class STEREOMIX_API USMLobbyWidget : public USMFrontendElementWidget
{
	GENERATED_BODY()

public:
	USMLobbyWidget();

protected:
	virtual void NativeConstruct() override;

	virtual void NativeOnActivated() override;

	virtual void NativeOnDeactivated() override;

	virtual void NativeDestruct() override;

	UFUNCTION()
	void OnCreateRoomButtonClicked();

	UFUNCTION()
	void OnQuickMatchButtonClicked();

	UFUNCTION()
	void OnJoinRoomButtonClicked();

	UFUNCTION()
	void OnSubmitCreateRoomOptions(USMPopup* Popup);

	UFUNCTION()
	void OnSubmitRoomCode(USMPopup* Popup);

	UFUNCTION()
	void OnClosePopup();

private:
	// ==================================================================
	// Components

	// UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess, BindWidget))
	// TObjectPtr<UCommonActivatableWidgetStack> LobbyPopupStack;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<USMCommonButton> CreateRoomButton;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<USMCommonButton> QuickMatchButton;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<USMCommonButton> JoinRoomButton;


	// ==================================================================
	// Frontend Elements

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Frontend Elements", meta = (AllowPrivateAccess))
	TSubclassOf<USMLobbyCreateRoomWidget> CreateRoomWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Frontend Elements", meta = (AllowPrivateAccess))
	TSubclassOf<USMLobbyQuickMatchWidget> QuickMatchWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Frontend Elements", meta = (AllowPrivateAccess))
	TSubclassOf<USMLobbyJoinRoomWidget> JoinRoomWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Frontend Elements", meta = (AllowPrivateAccess))
	TSubclassOf<USMCreateRoomPopup> CreateRoomPopupClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Frontend Elements", meta = (AllowPrivateAccess))
	TSubclassOf<USMLobbyRoomCodePopup> RoomCodePopupClass;


	// ==================================================================
	// Properties

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (AllowPrivateAccess))
	TObjectPtr<USMPopup> PopupWidget;

	UPROPERTY()
	TObjectPtr<USMClientLobbySubsystem> LobbySubsystem;

	UFUNCTION()
	void OnLobbyServiceStateChanged(EGrpcServiceState ServiceState);
};
