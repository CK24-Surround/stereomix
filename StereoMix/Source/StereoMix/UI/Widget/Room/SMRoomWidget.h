﻿// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonTextBlock.h"
#include "CommonUserWidget.h"
#include "Games/Room/SMRoomPlayerState.h"
#include "Games/Room/SMRoomState.h"
#include "SMRoomCodeCopyButton.h"
#include "SMRoomTeamWidget.h"
#include "UI/Widget/SMActivatableWidget.h"

#include "SMRoomWidget.generated.h"

class USMChatWidget;

/**
 *
 */
UCLASS(Abstract)
class STEREOMIX_API USMRoomWidget : public USMActivatableWidget
{
	GENERATED_BODY()

public:
	USMRoomWidget();

	void InitWidget(ASMRoomState* RoomState, ASMRoomPlayerState* PlayerState);

	ASMRoomState* GetOwningRoomState() const { return OwningRoomState.Get(); }

	ASMRoomPlayerState* GetOwningPlayerState() const { return OwningPlayerState.Get(); }

	bool bWaitForResponseTeamChange = false;

protected:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	virtual bool NativeOnHandleBackAction() override;

	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

	UFUNCTION(BlueprintCallable)
	void OpenChatInput();

	UFUNCTION()
	void UpdatePlayerCount() const;

	UFUNCTION()
	void OnGameStartButtonClicked();

	UFUNCTION()
	void OnQuitButtonClicked();

	UFUNCTION()
	void OnPlayerJoin(ASMPlayerState* JoinedPlayer);

	UFUNCTION()
	void OnPlayerLeft(ASMPlayerState* LeftPlayer);

	UFUNCTION()
	void OnTeamChangeResponse(bool bSuccess, ESMTeam NewTeam);

	UFUNCTION()
	void OnTeamPlayersUpdated(ESMTeam UpdatedTeam);

private:
	// =============================================================================
	// Bindings

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta = (BindWidget, AllowPrivateAccess))
	TObjectPtr<USMCommonButton> GameStartButton;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta = (BindWidget, AllowPrivateAccess))
	TObjectPtr<USMCommonButton> QuitButton;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta = (BindWidget, AllowPrivateAccess))
	TObjectPtr<UCommonTextBlock> PlayerCountTextBlock;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta = (BindWidget, AllowPrivateAccess))
	TObjectPtr<USMRoomCodeCopyButton> CodeCopyButton;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta = (BindWidget, AllowPrivateAccess))
	TObjectPtr<UImage> EdmCharacterImage;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta = (BindWidget, AllowPrivateAccess))
	TObjectPtr<UImage> FutureBassCharacterImage;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta = (BindWidget, AllowPrivateAccess))
	TObjectPtr<USMRoomTeamWidget> EdmTeamWidget;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta = (BindWidget, AllowPrivateAccess))
	TObjectPtr<USMRoomTeamWidget> FutureBassTeamWidget;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta = (BindWidget, AllowPrivateAccess))
	TObjectPtr<USMChatWidget> ChatWidget;


	// =============================================================================
	// Animations

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Animation", meta = (BindWidgetAnim, AllowPrivateAccess))
	TObjectPtr<UWidgetAnimation> TransitionAnim;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Animation", meta = (BindWidgetAnim, AllowPrivateAccess))
	TObjectPtr<UWidgetAnimation> EdmSelectAnim;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Animation", meta = (BindWidgetAnim, AllowPrivateAccess))
	TObjectPtr<UWidgetAnimation> FutureBassSelectAnim;


	// =============================================================================
	// Variables

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess))
	TArray<TObjectPtr<UTexture2D>> EdmCharacterTextures;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess))
	TArray<TObjectPtr<UTexture2D>> FutureBassCharacterTextures;

	UPROPERTY()
	TWeakObjectPtr<ASMRoomState> OwningRoomState;

	UPROPERTY()
	TWeakObjectPtr<ASMRoomPlayerState> OwningPlayerState;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Team", meta = (AllowPrivateAccess))
	ESMTeam CurrentTeam = ESMTeam::None;
};
