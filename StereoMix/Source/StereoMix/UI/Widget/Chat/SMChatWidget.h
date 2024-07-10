// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "CommonUserWidget.h"
#include "Games/Room/SMRoomState.h"
#include "UI/Widget/SMActivatableWidget.h"

#include "SMChatWidget.generated.h"

class ASMPlayerState;
class USMChatTypeButton;
class UCommonTextBlock;
class UInputAction;
class ASMPlayerController;
class UCommonListView;
class UEditableTextBox;

UENUM(BlueprintType)
enum class EChatType : uint8
{
	All,
	Team
};

/**
 *
 */
UCLASS(Abstract)
class STEREOMIX_API USMChatWidget : public USMActivatableWidget
{
	GENERATED_BODY()

public:
	USMChatWidget();

	void AddMessage(bool bTeamChat, const FString& Name, const FString& Message);

	EChatType GetChatType() const { return ChatType; }

	UFUNCTION()
	void SetChatType(EChatType NewChatType);

protected:
	virtual void NativePreConstruct() override;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	virtual void NativeOnActivated() override;

	virtual void NativeOnDeactivated() override;

	virtual UWidget* NativeGetDesiredFocusTarget() const override;

	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

	virtual FReply NativeOnKeyChar(const FGeometry& InGeometry, const FCharacterEvent& InCharEvent) override;

	/** 채팅 위젯을 엽니다. */
	UFUNCTION(BlueprintCallable)
	virtual void OpenChat();

	/** 채팅 타입을 변경합니다. */
	UFUNCTION(BlueprintCallable)
	virtual void ChangeChatType();

	UFUNCTION()
	virtual void OnOwningPlayerTeamChanged(ESMTeam NewTeam);

	UFUNCTION()
	virtual void OnChatReceivedFromServer(const FString& Name, const FString& ChatMessage);

	UFUNCTION()
	virtual void OnTeamChatReceivedFromServer(const FString& Name, const FString& ChatMessage);

	UFUNCTION()
	virtual void OnMessageInputBoxCommitted(const FText& Text, ETextCommit::Type CommitMethod);

private:
	void InitActivatableWidgets();

	void SetVisibilityForActivatableWidgets(ESlateVisibility InVisibility);

	// =============================================================================
	// Bindings

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta = (BindWidget, AllowPrivateAccess))
	TObjectPtr<USMChatTypeButton> ChatTypeButton;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta = (BindWidget, AllowPrivateAccess))
	TObjectPtr<UCommonListView> MessageListView;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components", meta = (BindWidget, AllowPrivateAccess))
	TObjectPtr<UEditableTextBox> MessageInputBox;


	// =============================================================================
	// Properties

	/** 채팅 위젯이 활성화 또는 비활성화될 때 함께 Visiblity 값이 바뀌는 위젯 목록입니다. 활성화 시 Visible로, 비활성화 시 Hidden으로 변경됩니다. */
	UPROPERTY(EditDefaultsOnly, Category = "Chat", meta = (AllowPrivateAccess))
	TArray<FWidgetChild> ActivatableWidgets;

	/** 비활성화 시 위젯을 숨김 처리할 지 여부를 나타냅니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chat", meta = (AllowPrivateAccess))
	bool bHideWidgetsOnDeactivate = true;

	/** 채팅 위젯을 항상 활성화 상태로 둘 것인지를 나타냅니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chat", meta = (AllowPrivateAccess))
	bool bAlwaysActivateWidget = false;

	/** 채팅 타입을 나타냅니다. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Chat", meta = (AllowPrivateAccess))
	EChatType ChatType = EChatType::All;

	/** 채팅 타입 토글 여부를 나타냅니다. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Chat", meta = (AllowPrivateAccess))
	bool bCanChangeChatType = false;

	UPROPERTY()
	TObjectPtr<ASMPlayerController> OwningPlayerController;

	UPROPERTY()
	TObjectPtr<ASMPlayerState> OwningPlayerState;

	FDelegateHandle ChatTypeButtonClickDelegateHandle;
};
