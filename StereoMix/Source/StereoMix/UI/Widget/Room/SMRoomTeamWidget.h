// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonListView.h"
#include "CommonUserWidget.h"
#include "Data/SMTeam.h"
#include "SMRoomPlayerEntry.h"
#include "UI/Animation/UmgAnimationDuration.h"
#include "UI/Widget/Common/SMButtonBase.h"

#include "SMRoomTeamWidget.generated.h"

class USMRoomWidget;

UENUM(BlueprintType)
enum class ERoomTeamSelectButtonState : uint8
{
	Idle,
	Focused,
	Selected,
	Disabled
};

/**
 *
 */
UCLASS(Abstract)
class STEREOMIX_API USMRoomTeamWidget : public USMButtonBase
{
	GENERATED_BODY()

public:
	USMRoomTeamWidget();

	void InitWidget(USMRoomWidget* RoomWidget, ESMTeam TargetTeam);

protected:
	virtual void NativePreConstruct() override;

	virtual void NativeConstruct() override;

	void SetButtonState(ERoomTeamSelectButtonState NewState);

	virtual void HandleFocusReceived() override;

	virtual void HandleFocusLost() override;

	virtual void NativeOnHovered() override;

	virtual void NativeOnUnhovered() override;

	virtual void NativeOnClicked() override;

	void OnSelected();
	void OnDeselected();

	UFUNCTION()
	void OnLocalPlayerTeamChangeResponse(bool bSuccess, ESMTeam NewTeam);

	UFUNCTION()
	void OnTeamPlayersUpdated(ESMTeam UpdatedTeam);

private:
	// =============================================================================
	// Animation

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Animation", meta = (BindWidgetAnim, AllowPrivateAccess))
	TObjectPtr<UWidgetAnimation> SelectAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess))
	FUmgAnimationDuration FocusAnimDuration;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess))
	FUmgAnimationDuration SelectAnimDuration;


	// =============================================================================
	// Bindings

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta = (BindWidget, AllowPrivateAccess))
	TObjectPtr<UImage> TeamCoverImage;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta = (BindWidget, AllowPrivateAccess))
	TObjectPtr<UImage> TeamLogoImage;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta = (BindWidget, AllowPrivateAccess))
	TObjectPtr<UCommonListView> PlayerListView;


	// =============================================================================
	// Variables

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Team", meta = (AllowPrivateAccess))
	TObjectPtr<UTexture2D> TeamCoverTexture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Team", meta = (AllowPrivateAccess))
	TObjectPtr<UTexture2D> TeamLogoTexture;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Widgets", meta = (AllowPrivateAccess))
	TWeakObjectPtr<USMRoomWidget> ParentRoomWidget;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Team", meta = (AllowPrivateAccess))
	ESMTeam Team = ESMTeam::None;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Team", meta = (AllowPrivateAccess))
	ERoomTeamSelectButtonState ButtonState = ERoomTeamSelectButtonState::Idle;
};
