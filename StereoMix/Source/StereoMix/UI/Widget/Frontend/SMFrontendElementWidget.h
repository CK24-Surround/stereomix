// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FMODEvent.h"
#include "UI/Widget/SMActivatableWidget.h"
#include "UI/Widget/Common/SMCommonButton.h"
#include "SMFrontendElementWidget.generated.h"

class USMFrontendWidget;

/**
 * StereoMix Frontend Widget
 */
UCLASS(Abstract)
class STEREOMIX_API USMFrontendElementWidget : public USMActivatableWidget
{
	GENERATED_BODY()

	friend class USMFrontendWidget;

public:
	USMFrontendElementWidget();

	UUMGSequencePlayer* PlayTransitionIn();

	UUMGSequencePlayer* PlayTransitionOut();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StereoMix Frontend")
	TObjectPtr<UFMODEvent> TransitionInSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StereoMix Frontend")
	TObjectPtr<UFMODEvent> TransitionOutSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StereoMix Frontend")
	FLinearColor BackgroundColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StereoMix Frontend")
	bool bAutoTransitionOnActivate = true;

	virtual void NativeConstruct() override;

	virtual void NativeOnActivated() override;

	virtual void NativeOnDeactivated() override;

	virtual void NativeDestruct() override;

	virtual bool NativeOnHandleBackAction() override;

	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

	USMFrontendWidget* GetParentFrontendWidget() const { return ParentFrontendWidget.Get(); }

	bool IsTransitioning() const { return bTransitioning; }

private:
	UPROPERTY(Transient, BlueprintReadOnly, meta=(BindWidgetOptional, AllowPrivateAccess))
	TObjectPtr<USMCommonButton> BackButton;

	UPROPERTY(Transient, BlueprintReadOnly, meta=(BindWidgetAnimOptional, AllowPrivateAccess))
	TObjectPtr<UWidgetAnimation> TransitionAnim;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess))
	TWeakObjectPtr<USMFrontendWidget> ParentFrontendWidget;

	UPROPERTY(Transient, BlueprintReadOnly, meta=(AllowPrivateAccess))
	bool bTransitioning = false;
};
