// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "SMFrontendWidgetStack.h"
#include "Components/Border.h"
#include "UI/Widget/CustomServer/SMCustomServerWidget.h"
#include "UI/Widget/Popup/SMAlertPopup.h"
#include "UI/Widget/Popup/SMPopup.h"
#include "SMFrontendWidget.generated.h"

USTRUCT(BlueprintType)
struct FFrontendBackgroundColorState
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
UCLASS(Abstract)
class STEREOMIX_API USMFrontendWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	USMFrontendWidget();

	void InitWidget(const FString& LevelOptions);
	
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

	FString RequestRoomCode;

protected:
	virtual void NativePreConstruct() override;

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	USMFrontendElementWidget* AddElementWidgetInternal(const TSubclassOf<USMFrontendElementWidget>& WidgetClass);

	void RemoveElementWidgetInternal(USMFrontendElementWidget* Widget);

private:
	void OnTransitionInFinished(UUMGSequencePlayer& SequencePlayer);
	
	UPROPERTY(Transient, BlueprintReadOnly, Category="Components", meta=(BindWidget, AllowPrivateAccess))
	TObjectPtr<USMFrontendWidgetStack> MainStack;

	UPROPERTY(Transient, BlueprintReadOnly, Category="Components", meta=(BindWidget, AllowPrivateAccess))
	TObjectPtr<UCommonActivatableWidgetStack> PopupStack;

	UPROPERTY(Transient, BlueprintReadOnly, Category="Components", meta=(BindWidget, AllowPrivateAccess))
	TObjectPtr<UBorder> Background;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Popup Elements", meta=(AllowPrivateAccess))
	TSubclassOf<USMAlertPopup> AlertPopupClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Frontend Elements", meta=(AllowPrivateAccess))
	TSubclassOf<USMFrontendElementWidget> LoginWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Frontend Elements", meta=(AllowPrivateAccess))
	TSubclassOf<USMFrontendElementWidget> MainMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Frontend Elements", meta=(AllowPrivateAccess))
	TSubclassOf<USMCustomServerWidget> CustomServerWidgetClass;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Background", meta=(AllowPrivateAccess))
	FFrontendBackgroundColorState BackgroundColorState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Background", meta=(AllowPrivateAccess))
	float BackgroundTransitionSpeed = 2.5f;

	UPROPERTY(Transient)
	TSubclassOf<USMFrontendElementWidget> InitWidgetClass;
};
