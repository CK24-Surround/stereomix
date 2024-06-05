// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMLobbyCreateRoomWidget.h"
#include "SMLobbyJoinRoomWidget.h"
#include "SMLobbyQuickMatchWidget.h"
#include "SMLobbyRoomCodePopup.h"
#include "Backend/Client/SMClientLobbySubsystem.h"
#include "UI/Widget/Common/SMCommonButton.h"
#include "UI/Widget/Frontend/SMFrontendElementWidget.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "SMLobbyWidget.generated.h"

/**
 * StereoMix Lobby Widget
 */
UCLASS()
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
	void OnSubmitRoomCode();

	UFUNCTION()
	void OnCloseRoomCodePopup();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Frontend Elements", meta=(AllowPrivateAccess))
	TSubclassOf<USMLobbyCreateRoomWidget> CreateRoomWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Frontend Elements", meta=(AllowPrivateAccess))
	TSubclassOf<USMLobbyQuickMatchWidget> QuickMatchWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Frontend Elements", meta=(AllowPrivateAccess))
	TSubclassOf<USMLobbyJoinRoomWidget> JoinRoomWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Frontend Elements", meta=(AllowPrivateAccess))
	TSubclassOf<USMLobbyRoomCodePopup> RoomCodePopupClass;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta=(AllowPrivateAccess, BindWidget))
	TObjectPtr<UCommonActivatableWidgetStack> LobbyPopupStack;
	
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta=(AllowPrivateAccess, BindWidget))
	TObjectPtr<USMCommonButton> CreateRoomButton;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta=(AllowPrivateAccess, BindWidget))
	TObjectPtr<USMCommonButton> QuickMatchButton;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta=(AllowPrivateAccess, BindWidget))
	TObjectPtr<USMCommonButton> JoinRoomButton;
	
	UPROPERTY(BlueprintReadOnly, Category="Widgets", meta=(AllowPrivateAccess))
	TWeakObjectPtr<USMLobbyRoomCodePopup> RoomCodePopup;

	UPROPERTY()
	TObjectPtr<USMClientLobbySubsystem> LobbySubsystem;

	UFUNCTION()
	void OnLobbyServiceStateChanged(EGrpcServiceState ServiceState);
};
