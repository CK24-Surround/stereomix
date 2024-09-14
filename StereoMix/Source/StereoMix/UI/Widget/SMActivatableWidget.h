// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"

#include "SMActivatableWidget.generated.h"

class USMActivatableWidgetStack;

UENUM(BlueprintType)
enum class ESMWidgetInputMode : uint8
{
	Default,
	GameAndMenu,
	Game,
	Menu
};

/**
 *
 */
UCLASS(ClassGroup = "StereoMix Common UI")
class STEREOMIX_API USMActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

	friend class USMActivatableWidgetStack;

public:
	UFUNCTION(BlueprintCallable, Category = "StereoMix Common UI")
	USMActivatableWidgetStack* GetRegisteredWidgetStack();

	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

#if WITH_EDITOR
	virtual void ValidateCompiledWidgetTree(const UWidgetTree& BlueprintWidgetTree, class IWidgetCompilerLog& CompileLog) const override;
#endif
	
protected:
	/** The desired input mode to use while this UI is activated, for example do you want key presses to still reach the game/player controller? */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	ESMWidgetInputMode InputConfig = ESMWidgetInputMode::Default;

	/** The desired mouse behavior when the game gets input. */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::CapturePermanently;
	
private:
	UPROPERTY(Transient)
	TObjectPtr<USMActivatableWidgetStack> RegisteredStack;
};
