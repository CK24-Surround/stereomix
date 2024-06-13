// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "SMFrontendWidgetStack.h"
#include "Components/Border.h"
#include "UI/Widget/Popup/SMAlertPopup.h"
#include "UI/Widget/Popup/SMPopup.h"
#include "SMFrontendWidget.generated.h"

USTRUCT()
struct FBackgroundColorState
{
	GENERATED_BODY()

	UPROPERTY(Transient, VisibleAnywhere)
	bool bIsTransitioning = false;

	UPROPERTY(Transient, VisibleAnywhere)
	FLinearColor CurrentColor = FLinearColor::Black;

	UPROPERTY(Transient, VisibleAnywhere)
	FLinearColor NextColor = FLinearColor::Black;

	UPROPERTY(EditDefaultsOnly)
	float TransitionSpeed = 5.0f;

	void ChangeBackgroundColor(FLinearColor NewColor);

	FLinearColor TransitionBackgroundColorOnTick(float DeltaTime);
};

/**
 * StereoMix Frontend Widget
 */
UCLASS()
class STEREOMIX_API USMFrontendWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Background")
	float BackgroundTransitionSpeed = 5.0f;

	FString RequestRoomCode;

	USMFrontendWidget();

	USMFrontendWidgetStack* GetMainStack() const { return MainStack; }

	UFUNCTION(BlueprintCallable)
	void AddElementWidget(TSubclassOf<USMFrontendElementWidget> WidgetClass);
	void AddElementWidget(TSubclassOf<USMFrontendElementWidget> WidgetClass, TFunctionRef<void(USMFrontendElementWidget&)> InstanceInitFunc);
	void AddElementWidgetInstance(USMFrontendElementWidget& WidgetInstance);

	UFUNCTION(BlueprintCallable)
	void RemoveElementWidget(USMFrontendElementWidget* Widget);

	UFUNCTION(BlueprintCallable)
	USMPopup* AddPopup(TSubclassOf<USMPopup> PopupClass);

	UFUNCTION(BlueprintCallable)
	USMAlertPopup* ShowAlert(const FString& AlertText);

	UFUNCTION(BlueprintCallable)
	void RemoveTopElementWidget();

	UFUNCTION(BlueprintCallable)
	void ChangeBackgroundColor(FLinearColor NewColor);

protected:
	virtual void NativePreConstruct() override;

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	USMFrontendElementWidget* AddElementWidgetInternal(const TSubclassOf<USMFrontendElementWidget>& WidgetClass);

	void RemoveElementWidgetInternal(USMFrontendElementWidget* Widget);

private:
	UPROPERTY(Transient, BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess))
	TObjectPtr<USMFrontendWidgetStack> MainStack;

	UPROPERTY(Transient, BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess))
	TObjectPtr<UCommonActivatableWidgetStack> PopupStack;

	UPROPERTY(Transient, BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess))
	TObjectPtr<UBorder> Background;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Frontend Elements", meta=(AllowPrivateAccess))
	TSubclassOf<USMAlertPopup> AlertPopupClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Frontend Elements", meta=(AllowPrivateAccess))
	TSubclassOf<USMFrontendElementWidget> LoginWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Frontend Elements", meta=(AllowPrivateAccess))
	TSubclassOf<USMFrontendElementWidget> MainMenuWidgetClass;

	FBackgroundColorState BackgroundColorState;

	void OnTransitionInFinished(UUMGSequencePlayer& SequencePlayer);
};
